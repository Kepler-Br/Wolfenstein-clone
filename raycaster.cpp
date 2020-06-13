#include "raycaster.h"

#include <iostream> // DELETEME
Raycaster::Raycaster(const World &world, Lookup_table &lookup_table):
    world(world),
    lookup(lookup_table)
{
}

Ray Raycaster::cast(glm::vec2 position, const int degree)
{
    float verticalGrid;        // horizotal or vertical coordinate of intersection
    float horizontalGrid;      // theoritically, this will be multiple of TILE_SIZE
                             // , but some trick did here might cause
                             // the values off by 1
    float distToNextVerticalGrid; // how far to the next bound (this is multiple of
    float distToNextHorizontalGrid; // tile size)
    float xIntersection;  // x and y intersections
    float yIntersection;
    float distToNextXIntersection;
    float distToNextYIntersection;

    int xGridIndex;        // the current cell that the ray is in
    int yGridIndex;

    float distToVerticalGridBeingHit;      // the distance of the x and y ray intersections from
    float distToHorizontalGridBeingHit;      // the viewpoint

    int castArc;
    const int block_size = this->world.get_block_size();
    castArc = degree;
    castArc = int(degree);

    if (castArc>=this->lookup.angle360)
        castArc-=this->lookup.angle360;
    // Ray is between 0 to 180 degree (1st and 2nd quadrant).
    if (castArc < 0)
    {
        castArc = this->lookup.angle360 + castArc;
    }
//std::cout << castArc << std::endl;
    // Ray is facing down
    if (castArc > this->lookup.angle0 && castArc < this->lookup.angle180)
    {
        // truncuate then add to get the coordinate of the FIRST grid (horizontal
        // wall) that is in front of the player (this is in pixel unit)
        // ROUNDED DOWN
        horizontalGrid = floor(position.y/block_size)*block_size  + block_size;
//        horizontalGrid = (position.y/block_size)*block_size  + block_size;

        // compute distance to the next horizontal wall
        distToNextHorizontalGrid = block_size;

        float xtemp = this->lookup.itan(castArc)*(horizontalGrid-position.y);
        // we can get the vertical distance to that wall by
        // (horizontalGrid-playerY)
        // we can get the horizontal distance to that wall by
        // 1/tan(arc)*verticalDistance
        // find the x interception to that wall
        xIntersection = xtemp + position.x;
    }
    // Else, the ray is facing up
    else
    {
        horizontalGrid = floor(position.y/block_size)*block_size;
//        horizontalGrid = (position.y/block_size)*block_size;
        distToNextHorizontalGrid = -block_size;

        float xtemp = this->lookup.itan(castArc)*(horizontalGrid - position.y);
        xIntersection = xtemp + position.x;

        horizontalGrid--;
    }
    // LOOK FOR HORIZONTAL WALL

    // If ray is directly facing right or left, then ignore it
    if (castArc==this->lookup.angle0 || castArc==this->lookup.angle180)
    {
        distToHorizontalGridBeingHit=99999999.0f;
    }
    // else, move the ray until it hits a horizontal wall
    else
    {
        distToNextXIntersection = this->lookup.x_step(castArc);
        while (true)
        {
            xGridIndex = floor(xIntersection/block_size);
            yGridIndex = floor(horizontalGrid/block_size);
            int mapIndex=floor(yGridIndex*this->world.get_world_dimensions().x+xGridIndex);

            // If we've looked as far as outside the map range, then bail out
            if ((xGridIndex>=this->world.get_world_dimensions().x) ||
                (yGridIndex>=this->world.get_world_dimensions().y) ||
                xGridIndex<0 || yGridIndex<0)
            {
                distToHorizontalGridBeingHit = 999999999.0f;
                break;
            }
            // If the grid is not an Opening, then stop
//            else if (this.fMap.charAt(mapIndex)!='O')
            if(this->world.get_block(mapIndex).is_solid_wall)
            {
                distToHorizontalGridBeingHit  = (xIntersection-position.x)*this->lookup.icos(castArc);
                break;
            }
            // Else, keep looking.  At this point, the ray is not blocked, extend the ray to the next grid
            else
            {
                xIntersection += distToNextXIntersection;
                horizontalGrid += distToNextHorizontalGrid;
            }
        }
    }


    // FOLLOW X RAY
    if (castArc < this->lookup.angle90 || castArc > this->lookup.angle270)
    {
        verticalGrid = block_size + floor(position.x/block_size)*block_size;
//        verticalGrid = block_size + (position.x/block_size)*block_size;
        distToNextVerticalGrid = block_size;

        float ytemp = this->lookup.tan(castArc)*(verticalGrid - position.x);
        yIntersection = ytemp + position.y;

    }
    // RAY FACING LEFT
    else
    {
        verticalGrid = floor(position.x/block_size)*block_size;
//        verticalGrid = (position.x/block_size)*block_size;
        distToNextVerticalGrid = -block_size;

        float ytemp = this->lookup.tan(castArc)*(verticalGrid - position.x);
        yIntersection = ytemp + position.y;

        verticalGrid--;

    }
      // LOOK FOR VERTICAL WALL
    if (castArc==this->lookup.angle90||castArc==this->lookup.angle270)
    {
        distToVerticalGridBeingHit = 99999999.0f;
    }
    else
    {
        distToNextYIntersection = this->lookup.y_step(castArc);
        while (true)
        {
            // compute current map position to inspect
            xGridIndex = floor(verticalGrid/block_size);
            yGridIndex = floor(yIntersection/block_size);

            int mapIndex=floor(yGridIndex*this->world.get_world_dimensions().x+xGridIndex);


            if ((xGridIndex>=this->world.get_world_dimensions().x) ||
                (yGridIndex>=this->world.get_world_dimensions().y) ||
                xGridIndex<0 || yGridIndex<0)
            {
                distToVerticalGridBeingHit = 99999999.0f;
                break;
            }
            else if (this->world.get_block(mapIndex).is_solid_wall)
            {
                distToVerticalGridBeingHit =(yIntersection-position.y)*this->lookup.isin(castArc);
                break;
            }
            else
            {
                yIntersection += distToNextYIntersection;
                verticalGrid += distToNextVerticalGrid;
            }
        }
    }
    if(distToHorizontalGridBeingHit < distToVerticalGridBeingHit)
    {
        return {0, {xIntersection, horizontalGrid}, (float)distToHorizontalGridBeingHit, direction_right};
    }
    else
    {
        return {0, {verticalGrid, yIntersection}, (float)distToVerticalGridBeingHit, direction_right};

    }
}
