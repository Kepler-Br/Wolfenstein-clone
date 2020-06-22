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


#include <glm/glm.hpp> // DELETEME

//Ray Raycaster::cast_one(const glm::vec2 &position, int degree)
//{
//    const int block_size = this->world.get_block_size();
//    if (degree >= this->lookup.angle360)
//        degree -= this->lookup.angle360;
//    if (degree < 0)
//        degree += this->lookup.angle360;

//    glm::vec2 A;
//    if (degree > this->lookup.angle0 && degree < this->lookup.angle180)
//        A.y = floor(position.y/block_size)*block_size + block_size;
//    // Else, the ray is facing up
//    else
//        A.y = floor(position.y/block_size)*block_size - 1;
//    A.x = position.x + (position.y - A.y)/ -this->lookup.tan(degree);

//    float dist_to_horizontal_grid_being_hit;
//    if (degree==this->lookup.angle0 || degree==this->lookup.angle180)
//        dist_to_horizontal_grid_being_hit=99999999.0f;
//    else
//        dist_to_horizontal_grid_being_hit = glm::length(A);

//    glm::vec2 B;
//    if (degree < this->lookup.angle90 || degree > this->lookup.angle270)
//        B.x = floor(position.x / block_size) * block_size + block_size;
//    // RAY FACING LEFT
//    else
//        B.x = floor(position.x / block_size) * block_size - 1;
//    B.y = position.y + (position.x - B.x)*-this->lookup.tan(degree);
//    float dist_to_vertical_grid_being_hit;
//    if (degree==this->lookup.angle90||degree==this->lookup.angle270)
//    {
//        dist_to_vertical_grid_being_hit = 99999999.0f;
//    }
//    else
//        dist_to_vertical_grid_being_hit = glm::length(B);

//    if(dist_to_horizontal_grid_being_hit < dist_to_vertical_grid_being_hit)
//        return Ray{.block_id=0, .position=A,
//                   .length=dist_to_horizontal_grid_being_hit,
//                   .hit_side=direction_up};
//    else
//        return Ray{.block_id=0, .position=B,
//                   .length=dist_to_vertical_grid_being_hit,
//                   .hit_side=direction_up};
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

Ray Raycaster::cast_one(const glm::vec2 &position, int degree)
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
            else if(/*this->world.get_block(mapIndex).is_solid_wall*/ true)
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
            else if (/*this->world.get_block(mapIndex).is_solid_wall*/ true)
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
            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_up};
        else
            return {(size_t)x_map_index, {x_intersection, horizontal_grid}, (float)dist_to_horizontal_grid_being_hit, direction_down};
    }
    else
    {
        const Block &block = this->world.get_block(y_map_index);
        const float moved_beginning_coordinate = vertical_grid - block.scaled_world_position.x;
        if(moved_beginning_coordinate > threshold)
            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_left};
        else
            return {(size_t)y_map_index, {vertical_grid, y_intersection}, (float)dist_to_vertical_grid_being_hit, direction_right};
    }
}
