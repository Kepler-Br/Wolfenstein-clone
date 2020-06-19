#include "raycaster.h"

#include <iostream> // DELETEME
Raycaster::Raycaster(const World &world, Lookup_table &lookup_table):
    world(world),
    lookup(lookup_table)
{
}

Ray Raycaster::cast(glm::vec2 position, int degree)
{
    float vertical_grid;        // horizotal or vertical coordinate of intersection
    float horizontal_grid;      // theoritically, this will be multiple of TILE_SIZE
                             // , but some trick did here might cause
                             // the values off by 1
    float dist_to_next_vertical_grid; // how far to the next bound (this is multiple of
    float dist_to_next_horizontal_grid; // tile size)
    float x_intersection;  // x and y intersections
    float y_intersection;
    float dist_to_next_x_intersection;
    float dist_to_next_y_intersection;

    int x_grid_index;        // the current cell that the ray is in
    int y_grid_index;

    int x_map_index = 0;
    int y_map_index = 0;

    float dist_to_vertical_grid_being_hit;      // the distance of the x and y ray intersections from
    float dist_to_horizontal_grid_being_hit;      // the viewpoint

    const int block_size = this->world.get_block_size();

    if (degree>=this->lookup.angle360)
        degree-=this->lookup.angle360;
    if (degree < 0)
    {
        degree = this->lookup.angle360 + degree;
    }
    // Ray is between 0 to 180 degree (1st and 2nd quadrant).
    // Ray is facing down
    if (degree > this->lookup.angle0 && degree < this->lookup.angle180)
    {
        // truncuate then add to get the coordinate of the FIRST grid (horizontal
        // wall) that is in front of the player (this is in pixel unit)
        // ROUNDED DOWN
        horizontal_grid = floor(position.y/block_size)*block_size  + block_size;
//        horizontalGrid = (position.y/block_size)*block_size  + block_size;

        // compute distance to the next horizontal wall
        dist_to_next_horizontal_grid = block_size;

        float xtemp = this->lookup.itan(degree)*(horizontal_grid-position.y);
        // we can get the vertical distance to that wall by
        // (horizontalGrid-playerY)
        // we can get the horizontal distance to that wall by
        // 1/tan(arc)*verticalDistance
        // find the x interception to that wall
        x_intersection = xtemp + position.x;
    }
    // Else, the ray is facing up
    else
    {
        horizontal_grid = floor(position.y/block_size)*block_size;
        dist_to_next_horizontal_grid = -block_size;

        float xtemp = this->lookup.itan(degree)*(horizontal_grid - position.y);
        x_intersection = xtemp + position.x;

        horizontal_grid--;
    }
    // LOOK FOR HORIZONTAL WALL

    // If ray is directly facing right or left, then ignore it
    if (degree==this->lookup.angle0 || degree==this->lookup.angle180)
    {
        dist_to_horizontal_grid_being_hit=99999999.0f;
    }
    // else, move the ray until it hits a horizontal wall
    else
    {
        dist_to_next_x_intersection = this->lookup.x_step(degree);
        while (true)
        {
            // compute current map position to inspect
            x_grid_index = floor(x_intersection/block_size);
            y_grid_index = floor(horizontal_grid/block_size);
            int mapIndex=floor(y_grid_index*this->world.get_world_dimensions().x+x_grid_index);

            // If we've looked as far as outside the map range, then bail out
            if ((x_grid_index>=this->world.get_world_dimensions().x) ||
                (y_grid_index>=this->world.get_world_dimensions().y) ||
                x_grid_index<0 || y_grid_index<0)
            {
                dist_to_horizontal_grid_being_hit = 999999999.0f;
                break;
            }
            // If the grid is not an Opening, then stop
            else if(this->world.get_block(mapIndex).is_solid_wall)
            {
                dist_to_horizontal_grid_being_hit = (x_intersection-position.x)*this->lookup.icos(degree);
                x_map_index = mapIndex;
                break;
            }
            // Else, keep looking.  At this point, the ray is not blocked, extend the ray to the next grid
            else
            {
                x_intersection += dist_to_next_x_intersection;
                horizontal_grid += dist_to_next_horizontal_grid;
            }

        }
    }


    // FOLLOW X RAY
    if (degree < this->lookup.angle90 || degree > this->lookup.angle270)
    {
        vertical_grid = block_size + floor(position.x/block_size)*block_size;
//        verticalGrid = block_size + (position.x/block_size)*block_size;
        dist_to_next_vertical_grid = block_size;

        float ytemp = this->lookup.tan(degree)*(vertical_grid - position.x);
        y_intersection = ytemp + position.y;

    }
    // RAY FACING LEFT
    else
    {
        vertical_grid = floor(position.x/block_size)*block_size;
//        verticalGrid = (position.x/block_size)*block_size;
        dist_to_next_vertical_grid = -block_size;

        float ytemp = this->lookup.tan(degree)*(vertical_grid - position.x);
        y_intersection = ytemp + position.y;

        vertical_grid--;

    }
      // LOOK FOR VERTICAL WALL
    if (degree==this->lookup.angle90||degree==this->lookup.angle270)
    {
        dist_to_vertical_grid_being_hit = 99999999.0f;
    }
    else
    {
        dist_to_next_y_intersection = this->lookup.y_step(degree);
        while (true)
        {
            // compute current map position to inspect
            x_grid_index = floor(vertical_grid/block_size);
            y_grid_index = floor(y_intersection/block_size);

            int mapIndex=floor(y_grid_index*this->world.get_world_dimensions().x+x_grid_index);


            if ((x_grid_index>=this->world.get_world_dimensions().x) ||
                (y_grid_index>=this->world.get_world_dimensions().y) ||
                x_grid_index<0 || y_grid_index<0)
            {
                dist_to_vertical_grid_being_hit = 99999999.0f;
                break;
            }
            else if (this->world.get_block(mapIndex).is_solid_wall)
            {
                dist_to_vertical_grid_being_hit = (y_intersection-position.y)*this->lookup.isin(degree);
                y_map_index = mapIndex;
                break;
            }
            else
            {
                y_intersection += dist_to_next_y_intersection;
                vertical_grid += dist_to_next_vertical_grid;
            }
        }
    }
    const float threshold = 0.5f * this->world.get_block_size();
    if(dist_to_horizontal_grid_being_hit < dist_to_vertical_grid_being_hit)
    {
        const Block &block = this->world.get_block(x_map_index);
        const float moved_beginning_coordinate = horizontal_grid - block.scaled_world_position.y;
        if(moved_beginning_coordinate > threshold)
            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_right};
        else
            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_left};
    }
    else
    {
        const Block &block = this->world.get_block(y_map_index);
        const float moved_beginning_coordinate = vertical_grid - block.scaled_world_position.x;
        if(moved_beginning_coordinate > threshold)
            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_up};
        else
            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_down};
    }
}

Ray Raycaster::cast(glm::vec2 position, int degree, int depth)
{
    float vertical_grid;        // horizotal or vertical coordinate of intersection
    float horizontal_grid;      // theoritically, this will be multiple of TILE_SIZE
                             // , but some trick did here might cause
                             // the values off by 1
    float dist_to_next_vertical_grid; // how far to the next bound (this is multiple of
    float dist_to_next_horizontal_grid; // tile size)
    float x_intersection;  // x and y intersections
    float y_intersection;
    float dist_to_next_x_intersection;
    float dist_to_next_y_intersection;

    int x_grid_index;        // the current cell that the ray is in
    int y_grid_index;

    int x_map_index = 0;
    int y_map_index = 0;

    float dist_to_vertical_grid_being_hit;      // the distance of the x and y ray intersections from
    float dist_to_horizontal_grid_being_hit;      // the viewpoint

    const int block_size = this->world.get_block_size();
    int current_depth = depth;
    if (current_depth == 0)
        current_depth = 999;

    if (degree>=this->lookup.angle360)
        degree-=this->lookup.angle360;
    if (degree < 0)
        degree = this->lookup.angle360 + degree;
    // Ray is between 0 to 180 degree (1st and 2nd quadrant).
    // Ray is facing down
    if (degree > this->lookup.angle0 && degree < this->lookup.angle180)
    {
        // truncuate then add to get the coordinate of the FIRST grid (horizontal
        // wall) that is in front of the player (this is in pixel unit)
        // ROUNDED DOWN
        horizontal_grid = floor(position.y/block_size)*block_size  + block_size;
//        horizontalGrid = (position.y/block_size)*block_size  + block_size;

        // compute distance to the next horizontal wall
        dist_to_next_horizontal_grid = block_size;

        float xtemp = this->lookup.itan(degree)*(horizontal_grid-position.y);
        // we can get the vertical distance to that wall by
        // (horizontalGrid-playerY)
        // we can get the horizontal distance to that wall by
        // 1/tan(arc)*verticalDistance
        // find the x interception to that wall
        x_intersection = xtemp + position.x;
    }
    // Else, the ray is facing up
    else
    {
        horizontal_grid = floor(position.y/block_size)*block_size;
        dist_to_next_horizontal_grid = -block_size;

        float xtemp = this->lookup.itan(degree)*(horizontal_grid - position.y);
        x_intersection = xtemp + position.x;

        horizontal_grid--;
    }
    // LOOK FOR HORIZONTAL WALL

    // If ray is directly facing right or left, then ignore it
    if (degree==this->lookup.angle0 || degree==this->lookup.angle180)
    {
        dist_to_horizontal_grid_being_hit=99999999.0f;
    }
    // else, move the ray until it hits a horizontal wall
    else
    {
        dist_to_next_x_intersection = this->lookup.x_step(degree);

        while (--current_depth)
        {
            // compute current map position to inspect
            x_grid_index = floor(x_intersection/block_size);
            y_grid_index = floor(horizontal_grid/block_size);
            int mapIndex=floor(y_grid_index*this->world.get_world_dimensions().x+x_grid_index);

            // If we've looked as far as outside the map range, then bail out
            if ((x_grid_index>=this->world.get_world_dimensions().x) ||
                (y_grid_index>=this->world.get_world_dimensions().y) ||
                x_grid_index<0 || y_grid_index<0)
            {
                dist_to_horizontal_grid_being_hit = 999999999.0f;
                break;
            }
            // If the grid is not an Opening, then stop
            else if(this->world.get_block(mapIndex).is_solid_wall)
            {
                dist_to_horizontal_grid_being_hit = (x_intersection-position.x)*this->lookup.icos(degree);
                x_map_index = mapIndex;
                break;
            }
            // Else, keep looking.  At this point, the ray is not blocked, extend the ray to the next grid
            else
            {
                x_intersection += dist_to_next_x_intersection;
                horizontal_grid += dist_to_next_horizontal_grid;
            }
        }
    }
    current_depth = depth;
    if (current_depth == 0)
        current_depth = 999;

    // FOLLOW X RAY
    if (degree < this->lookup.angle90 || degree > this->lookup.angle270)
    {
        vertical_grid = block_size + floor(position.x/block_size)*block_size;
//        verticalGrid = block_size + (position.x/block_size)*block_size;
        dist_to_next_vertical_grid = block_size;

        float ytemp = this->lookup.tan(degree)*(vertical_grid - position.x);
        y_intersection = ytemp + position.y;

    }
    // RAY FACING LEFT
    else
    {
        vertical_grid = floor(position.x/block_size)*block_size;
//        verticalGrid = (position.x/block_size)*block_size;
        dist_to_next_vertical_grid = -block_size;

        float ytemp = this->lookup.tan(degree)*(vertical_grid - position.x);
        y_intersection = ytemp + position.y;

        vertical_grid--;

    }
      // LOOK FOR VERTICAL WALL
    if (degree==this->lookup.angle90||degree==this->lookup.angle270)
    {
        dist_to_vertical_grid_being_hit = 99999999.0f;
    }
    else
    {
        dist_to_next_y_intersection = this->lookup.y_step(degree);
        while (--current_depth)
        {
            // compute current map position to inspect
            x_grid_index = floor(vertical_grid/block_size);
            y_grid_index = floor(y_intersection/block_size);

            int mapIndex=floor(y_grid_index*this->world.get_world_dimensions().x+x_grid_index);


            if ((x_grid_index>=this->world.get_world_dimensions().x) ||
                (y_grid_index>=this->world.get_world_dimensions().y) ||
                x_grid_index<0 || y_grid_index<0)
            {
                dist_to_vertical_grid_being_hit = 99999999.0f;
                break;
            }
            else if (this->world.get_block(mapIndex).is_solid_wall)
            {
                dist_to_vertical_grid_being_hit = (y_intersection-position.y)*this->lookup.isin(degree);
                y_map_index = mapIndex;
                break;
            }
            else
            {
                y_intersection += dist_to_next_y_intersection;
                vertical_grid += dist_to_next_vertical_grid;
            }
        }
    }
    const float threshold = 0.5f * this->world.get_block_size();
    if(dist_to_horizontal_grid_being_hit < dist_to_vertical_grid_being_hit)
    {
        const Block &block = this->world.get_block(x_map_index);
        const float moved_beginning_coordinate = horizontal_grid - block.scaled_world_position.y;
        if(moved_beginning_coordinate > threshold)
            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_right};
        else
            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_left};
    }
    else
    {
        const Block &block = this->world.get_block(y_map_index);
        const float moved_beginning_coordinate = vertical_grid - block.scaled_world_position.x;
        if(moved_beginning_coordinate > threshold)
            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_up};
        else
            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_down};
    }
}

#include <glm/glm.hpp> // DELETEME

Ray Raycaster::cast_one(const glm::vec2 &start_position, int degree)
{
    int horizontal_map_index, depth_of_field;
    glm::ivec2 map_xy;
    glm::vec2 offset;
    glm::vec2 ray_position;
    const float epsilon = 0.01f;
    const int &block_size = this->world.get_block_size();

    // Horizontal line check.
    float distance_horizontal = INFINITY;
    glm::vec2 horizontal_ray = start_position;
//    float a_tan = -1.0f/std::tan(degree);
    float a_tan = this->lookup.itan(degree);
    if (degree>=this->lookup.angle360)
        degree-=this->lookup.angle360;
    if (degree < 0)
        degree = this->lookup.angle360 + degree;
    depth_of_field = 0;
    if (degree==this->lookup.angle0 || degree==this->lookup.angle180) // Looking straight left or right.
    {
        degree += epsilon;
    }
    if (degree > this->lookup.angle0 && degree < this->lookup.angle180) // Looking down.
    {
        ray_position.y = (int(int(start_position.y)/block_size)*block_size) - 0.0001f;
        ray_position.x = (start_position.y - ray_position.y)*a_tan + start_position.x;
        offset.y = -block_size;
        offset.x = -offset.y*a_tan;
    }
    else // Looking up.
    {
        ray_position.y = int(int(start_position.y)/block_size)*block_size + block_size;
        ray_position.x = (start_position.y - ray_position.y)*a_tan + start_position.x;
        offset.y = block_size;
        offset.x = -offset.y*a_tan;
    }

    const glm::ivec2 &world_size = this->world.get_world_dimensions();
    while (depth_of_field < 18)
    {
        map_xy.x = int(ray_position.x)/block_size;
        map_xy.y = int(ray_position.y)/block_size;
        horizontal_map_index = map_xy.y * world_size.x + map_xy.x;
        if (horizontal_map_index >= 0 && horizontal_map_index < world_size.x * world_size.y && this->world.get_block(horizontal_map_index).is_solid_wall)
        {
            depth_of_field = 18;// Hit wall.
            horizontal_ray = ray_position;
            distance_horizontal = glm::length(start_position - ray_position);
        }
        else // Next wall.
        {
            ray_position.x += offset.x;
            ray_position.y += offset.y;
            depth_of_field += 1;
            distance_horizontal = glm::length(start_position - ray_position);
        }
    }

    // Vertical line check.
    float distance_vertical = INFINITY;
    glm::vec2 vertical_ray = start_position;
//    float negative_tan = -std::tan(degree);
    float negative_tan = -this->lookup.tan(degree);
//    float P2 = M_PI/ 2.0f;
//    float P3 = 3.0f*M_PI/2.0f;
    int vertical_map_index = 0;
    if (degree==this->lookup.angle90||degree==this->lookup.angle270) // Up or down.
    {
        degree += epsilon;
    }
    if (degree < this->lookup.angle90 || degree > this->lookup.angle270) // Looking right.
    {
        ray_position.x = (int(int(start_position.x)/block_size)*block_size) + block_size;
        ray_position.y = (start_position.x - ray_position.x)*negative_tan + start_position.y;
        offset.x = block_size;
        offset.y = -offset.x*negative_tan;
    }
    else // Looking left.
    {
        ray_position.x = (int(int(start_position.x)/block_size)*block_size) - 0.0001f;
        ray_position.y = (start_position.x - ray_position.x)*negative_tan + start_position.y;
        offset.x = -block_size;
        offset.y = -offset.x*negative_tan;
    }
    depth_of_field = 0;

    while (depth_of_field < 18)
    {
        map_xy.x = (int)(ray_position.x/block_size);
        map_xy.y = (int)(ray_position.y/block_size);
        vertical_map_index = map_xy.y * world_size.x + map_xy.x;
        if (vertical_map_index >= 0 && vertical_map_index < world_size.x * world_size.y && this->world.get_block(vertical_map_index).is_solid_wall)
        {
            depth_of_field = 18;// Hit wall.
            vertical_ray = ray_position;
            distance_vertical = glm::length(start_position - ray_position);
        }
        else // Next wall.
        {
            ray_position.x += offset.x;
            ray_position.y += offset.y;
            depth_of_field += 1;
            distance_vertical = glm::length(start_position - ray_position);
        }
    }
    const float threshold = 0.5f * this->world.get_block_size();
    if (distance_vertical < distance_horizontal)
    {
        const float moved_beginning_coordinate = vertical_ray.x - this->world.get_block(vertical_map_index).scaled_world_position.x;
        if(moved_beginning_coordinate > threshold)
            return {(size_t)vertical_map_index, vertical_ray, distance_vertical, direction_left};
        else
            return {(size_t)vertical_map_index, vertical_ray, distance_vertical, direction_right};
    }
    else
    {
        const float moved_beginning_coordinate = horizontal_ray.y - this->world.get_block(horizontal_map_index).scaled_world_position.y;
        if(moved_beginning_coordinate > threshold)
            return {(size_t)horizontal_map_index, horizontal_ray, distance_horizontal, direction_up};
        else
            return {(size_t)horizontal_map_index, horizontal_ray, distance_horizontal, direction_down};
//        return {horizontal_map_index, horizontal_ray, distance_horizontal, direction_up};
    }
}

//Ray Raycaster::cast_one(const glm::vec2 &position, int degree)
//{
//    float vertical_grid;        // horizotal or vertical coordinate of intersection
//    float horizontal_grid;      // theoritically, this will be multiple of TILE_SIZE
//                             // , but some trick did here might cause
//                             // the values off by 1
//    float dist_to_next_vertical_grid; // how far to the next bound (this is multiple of
//    float dist_to_next_horizontal_grid; // tile size)
//    float x_intersection;  // x and y intersections
//    float y_intersection;
//    float dist_to_next_x_intersection;
//    float dist_to_next_y_intersection;

//    int x_grid_index;        // the current cell that the ray is in
//    int y_grid_index;

//    int x_map_index = 0;
//    int y_map_index = 0;

//    float dist_to_vertical_grid_being_hit;      // the distance of the x and y ray intersections from
//    float dist_to_horizontal_grid_being_hit;      // the viewpoint

//    const int block_size = this->world.get_block_size();

//    if (degree>=this->lookup.angle360)
//        degree-=this->lookup.angle360;
//    if (degree < 0)
//        degree = this->lookup.angle360 + degree;
//    // Ray is between 0 to 180 degree (1st and 2nd quadrant).
//    // Ray is facing down
//    if (degree > this->lookup.angle0 && degree < this->lookup.angle180)
//    {
//        // truncuate then add to get the coordinate of the FIRST grid (horizontal
//        // wall) that is in front of the player (this is in pixel unit)
//        // ROUNDED DOWN
//        horizontal_grid = floor(position.y/block_size)*block_size  + block_size;
////        horizontalGrid = (position.y/block_size)*block_size  + block_size;

//        // compute distance to the next horizontal wall
//        dist_to_next_horizontal_grid = block_size;

//        float xtemp = this->lookup.itan(degree)*(horizontal_grid-position.y);
//        // we can get the vertical distance to that wall by
//        // (horizontalGrid-playerY)
//        // we can get the horizontal distance to that wall by
//        // 1/tan(arc)*verticalDistance
//        // find the x interception to that wall
//        x_intersection = xtemp + position.x;
//    }
//    // Else, the ray is facing up
//    else
//    {
//        horizontal_grid = floor(position.y/block_size)*block_size;
//        dist_to_next_horizontal_grid = -block_size;

//        float xtemp = this->lookup.itan(degree)*(horizontal_grid - position.y);
//        x_intersection = xtemp + position.x;

//        horizontal_grid--;
//    }
//    // LOOK FOR HORIZONTAL WALL

//    // If ray is directly facing right or left, then ignore it
//    if (degree==this->lookup.angle0 || degree==this->lookup.angle180)
//    {
//        dist_to_horizontal_grid_being_hit=99999999.0f;
//    }
//    else
//    {
//        dist_to_next_x_intersection = this->lookup.x_step(degree);
//        dist_to_horizontal_grid_being_hit = (x_intersection-position.x)*this->lookup.icos(degree);
//        x_grid_index = floor(x_intersection/block_size);
//        y_grid_index = floor(horizontal_grid/block_size);
//        int mapIndex=floor(y_grid_index*this->world.get_world_dimensions().x+x_grid_index);

//        if ((x_grid_index>=this->world.get_world_dimensions().x) ||
//            (y_grid_index>=this->world.get_world_dimensions().y) ||
//            x_grid_index<0 || y_grid_index<0)
//        {
//            dist_to_horizontal_grid_being_hit = 999999999.0f;
//        }
//        else
//        {
//            dist_to_horizontal_grid_being_hit = (x_intersection-position.x)*this->lookup.icos(degree);
//            x_map_index = mapIndex;
//        }
//    }


//    // FOLLOW X RAY
//    if (degree < this->lookup.angle90 || degree > this->lookup.angle270)
//    {
//        vertical_grid = block_size + floor(position.x/block_size)*block_size;
////        vertical_grid = block_size + (position.x/block_size)*block_size;
//        dist_to_next_vertical_grid = block_size;

//        float ytemp = this->lookup.tan(degree)*(vertical_grid - position.x);
//        y_intersection = ytemp + position.y;

//    }
//    // RAY FACING LEFT
//    else
//    {
//        vertical_grid = floor(position.x/block_size)*block_size;
////        verticalGrid = (position.x/block_size)*block_size;
//        dist_to_next_vertical_grid = -block_size;

//        float ytemp = this->lookup.tan(degree)*(vertical_grid - position.x);
//        y_intersection = ytemp + position.y;

//        vertical_grid--;

//    }
//      // LOOK FOR VERTICAL WALL
//    if (degree==this->lookup.angle90||degree==this->lookup.angle270)
//    {
//        dist_to_vertical_grid_being_hit = 99999999.0f;
//    }
//    else
//    {
//        dist_to_next_y_intersection = this->lookup.y_step(degree);
//        dist_to_vertical_grid_being_hit = (y_intersection-position.y)*this->lookup.isin(degree);
//        x_grid_index = floor(vertical_grid/block_size);
//        y_grid_index = floor(y_intersection/block_size);

//        int mapIndex=floor(y_grid_index*this->world.get_world_dimensions().x+x_grid_index);

//        if ((x_grid_index>=this->world.get_world_dimensions().x) ||
//            (y_grid_index>=this->world.get_world_dimensions().y) ||
//            x_grid_index<0 || y_grid_index<0)
//        {
//            dist_to_vertical_grid_being_hit = 99999999.0f;
//        }
//        else
//        {
//            dist_to_vertical_grid_being_hit = (y_intersection-position.y)*this->lookup.isin(degree);
//            y_map_index = mapIndex;
//        }
//    }
//    const float threshold = 0.5f * this->world.get_block_size();
//    if(dist_to_horizontal_grid_being_hit < dist_to_vertical_grid_being_hit)
//    {
//        const Block &block = this->world.get_block(x_map_index);
//        const float moved_beginning_coordinate = horizontal_grid - block.scaled_world_position.y;
//        if(moved_beginning_coordinate > threshold)
//            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_right};
//        else
//            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_left};
//    }
//    else
//    {
//        const Block &block = this->world.get_block(y_map_index);
//        const float moved_beginning_coordinate = vertical_grid - block.scaled_world_position.x;
//        if(moved_beginning_coordinate > threshold)
//            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_up};
//        else
//            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_down};
//    }
//}
