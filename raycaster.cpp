#include "raycaster.h"
#include <iostream>

Raycaster::Raycaster(const World &world): world(world)
{
}

const Ray Raycaster::raycast(float ray_angle, const glm::vec2 &start_position) const
{
    int horizontal_map_index, depth_of_field;
    glm::ivec2 map_xy;
    glm::vec2 offset;
    glm::vec2 ray_position;
    const float epsilon = 0.01f;
    const float &block_size = this->world.get_block_size();

    // Horizontal line check.
    float distance_horizontal = INFINITY;
    glm::vec2 horizontal_ray = start_position;
    float a_tan = -1.0f/std::tan(ray_angle);
    if (ray_angle < 0.0f)
        ray_angle += M_PI*2.0f;
    if (ray_angle > M_PI*2.0f)
        ray_angle -= M_PI*2.0f;
    depth_of_field = 0;
    if (ray_angle == 0.0f || ray_angle == M_PI) // Looking straight left or right.
    {
        ray_angle += epsilon;
    }
    if (ray_angle > M_PI ) // Looking down.
    {
        ray_position.y = (int(int(start_position.y)/block_size)*block_size) - 0.0001f;
        ray_position.x = (start_position.y - ray_position.y)*a_tan + start_position.x;
        offset.y = -block_size;
        offset.x = -offset.y*a_tan;
    }
    if (ray_angle < M_PI ) // Looking up.
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
    float negative_tan = -std::tan(ray_angle);
    float P2 = M_PI/ 2.0f;
    float P3 = 3.0f*M_PI/2.0f;
    int vertical_map_index = 0;
    if (ray_angle == 0 || ray_angle == M_PI) // Up or down.
    {
        ray_angle += epsilon;
    }
    if (ray_angle > P2 && ray_angle < P3) // Looking left.
    {
        ray_position.x = (int(int(start_position.x)/block_size)*block_size) - 0.0001f;
        ray_position.y = (start_position.x - ray_position.x)*negative_tan + start_position.y;
        offset.x = -block_size;
        offset.y = -offset.x*negative_tan;
    }
    if (ray_angle < P2 || ray_angle > P3 ) // Looking right.
    {
        ray_position.x = (int(int(start_position.x)/block_size)*block_size) + block_size;
        ray_position.y = (start_position.x - ray_position.x)*negative_tan + start_position.y;
        offset.x = block_size;
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
            return {vertical_map_index, vertical_ray, distance_vertical, direction_left};
        else
            return {vertical_map_index, vertical_ray, distance_vertical, direction_right};
    }
    else
    {
        const float moved_beginning_coordinate = horizontal_ray.y - this->world.get_block(horizontal_map_index).scaled_world_position.y;
        if(moved_beginning_coordinate > threshold)
            return {horizontal_map_index, horizontal_ray, distance_horizontal, direction_up};
        else
            return {horizontal_map_index, horizontal_ray, distance_horizontal, direction_down};
//        return {horizontal_map_index, horizontal_ray, distance_horizontal, direction_up};
    }
}

const Ray Raycaster::raycast(const glm::vec2 &ray_direction, const glm::vec2 &start_position) const
{
    return this->raycast((float)atan2(ray_direction.y, ray_direction.x), start_position);
}

const Ray Raycaster::raycast_one(float ray_angle, const glm::vec2 &start_position) const
{
    glm::ivec2 map_xy;
    glm::vec2 ray_position;
    const glm::ivec2 &world_size = this->world.get_world_dimensions();

    // Horizontal line check.
    glm::vec2 horizontal_ray = start_position;
    float a_tan = -1.0f/std::tan(ray_angle);
    if (ray_angle < 0.0f)
        ray_angle += M_PI*2.0f;
    if (ray_angle > M_PI*2.0f)
        ray_angle -= M_PI*2.0f;
    if (ray_angle > M_PI ) // Looking down.
    {
        horizontal_ray.y = (((int)start_position.y>>6)<<6) - 0.0001f;
        horizontal_ray.x = (start_position.y - horizontal_ray.y)*a_tan + start_position.x;
    }
    if (ray_angle < M_PI ) // Looking up.
    {
        horizontal_ray.y = (((int)start_position.y>>6)<<6) + 64.0f;
        horizontal_ray.x = (start_position.y - horizontal_ray.y)*a_tan + start_position.x;
    }
    if (ray_angle == 0 || ray_angle == M_PI) // Looking straight left or right.
    {
        horizontal_ray = start_position;
    }
    float distance_horizontal = glm::length(start_position - horizontal_ray);
    map_xy.x = (int)(ray_position.x) >> 6;
    map_xy.y = (int)(ray_position.y) >> 6;
    int horizontal_map_index = map_xy.y * world_size.x + map_xy.x;

    // Vertical line check.
    glm::vec2 vertical_ray = start_position;
    float negative_tan = -std::tan(ray_angle);
    float P2 = M_PI/ 2.0f;
    float P3 = 3.0f*M_PI/2.0f;
    if (ray_angle > P2 && ray_angle < P3) // Looking left.
    {
        vertical_ray.x = (((int)start_position.x>>6)<<6) - 0.0001f;
        vertical_ray.y = (start_position.x - vertical_ray.x)*negative_tan + start_position.y;
    }
    if (ray_angle < P2 || ray_angle > P3 ) // Looking right.
    {
        vertical_ray.x = (((int)start_position.x>>6)<<6) + 64.0f;
        vertical_ray.y = (start_position.x - vertical_ray.x)*negative_tan + start_position.y;
    }
    if (ray_angle == 0 || ray_angle == M_PI) // Up or down.
    {
        vertical_ray = start_position;
    }
    float distance_vertical = glm::length(start_position - vertical_ray);
    map_xy.x = (int)(ray_position.x) >> 6;
    map_xy.y = (int)(ray_position.y) >> 6;
    int vertical_map_index = map_xy.y * world_size.x + map_xy.x;
    if (distance_vertical < distance_horizontal)
    {
        return {vertical_map_index, vertical_ray, distance_vertical};
    }
    else
    {
        return {horizontal_map_index, horizontal_ray, distance_horizontal};
    }
}

const Ray Raycaster::raycast_one(const glm::vec2 &ray_direction, const glm::vec2 &start_position) const
{
    return this->raycast_one((float)atan2(ray_direction.y, ray_direction.x), start_position);
}