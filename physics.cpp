#include "physics.h"

Physics::Physics(const World &world, const Raycaster &raycaster, const Player &player):
    world(world),
    raycaster(raycaster),
    player(player)
{
    this->rays = new Ray[this->ray_count];
}

Physics::~Physics()
{
    delete[] this->rays;
}

const glm::vec3 Physics::handle_wall_colisions(const glm::vec3 &current_position, const glm::vec3 &velocity) const
{
    float min_distance = INFINITY;
    int min_index = 0;
    const float stop_length = 10.0f;
    int less_stop_length_count = 0;
    for(int i = 0; i < ray_count; i++)
    {
        const Block &block = this->world.get_block(rays[i].block_index);
        if(block.is_passable)
            continue;
        if(rays[i].ray_length < min_distance)
        {
            min_distance = rays[i].ray_length;
            min_index = i;
        }
        if(rays[i].ray_length < stop_length)
            less_stop_length_count++;
    }
    if(less_stop_length_count > 1)
    {
        Ray ray = this->raycaster.raycast(velocity, current_position);
        if (ray.ray_length < stop_length)
            return current_position;
    }

    if(min_distance < stop_length)
    {
        glm::vec3 projected = glm::vec3(0);
        glm::vec3 new_velocity = glm::vec3(0);
        if(min_index == direction_right)
        {
            constexpr glm::vec3 normal = glm::vec3(1.0f, 0.0f, 0.0f);
            const float dot = glm::dot(velocity, normal);
            if (dot < 0.0f)
                new_velocity = velocity;
            else
                new_velocity = velocity - normal * glm::dot(velocity, normal);
        }
        if(min_index == direction_up)
        {
            constexpr glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
            const float dot = glm::dot(velocity, normal);
            if (dot < 0.0f)
                new_velocity = velocity;
            else
                new_velocity = velocity - normal * glm::dot(velocity, normal);
        }
        if(min_index == direction_left)
        {
            constexpr glm::vec3 normal = glm::vec3(-1.0f, 0.0f, 0.0f);
            const float dot = glm::dot(velocity, normal);
            if (dot < 0.0f)
                new_velocity = velocity;
            else
                new_velocity = velocity - normal * glm::dot(velocity, normal);
        }
        if(min_index == direction_down)
        {
            constexpr glm::vec3 normal = glm::vec3(0.0f, -1.0f, 0.0f);
            const float dot = glm::dot(velocity, normal);
            if (dot < 0.0f)
                new_velocity = velocity;
            else
                new_velocity = velocity - normal * glm::dot(velocity, normal);
        }
        return glm::vec3(current_position + new_velocity);
    }
    return current_position + velocity;
}

#include <iostream>

const glm::vec3 Physics::handle_portals(const glm::vec3 &current_position, const glm::vec3 &new_position) const
{
    const float &block_size = this->world.get_block_size();
//    const int current_block_index = (int)current_position.y/64 * this->world.get_world_dimensions().x + (int)current_position.x/64;
    const int current_block_index = (int)current_position.y/(int)block_size * this->world.get_world_dimensions().x + (int)current_position.x/(int)block_size;
//    const int new_block_index =  (int)new_position.y/64 * this->world.get_world_dimensions().x + (int)new_position.x/64;
    const int new_block_index =  (int)new_position.y/(int)block_size * this->world.get_world_dimensions().x + (int)new_position.x/(int)block_size;
    const Block &current_block = this->world.get_block(current_block_index);
    const Block &new_block = this->world.get_block(new_block_index);
    glm::vec3 translated_position = new_position;
//    const glm::vec2 delta = glm::vec2(current_block.world_position - new_block.world_position);
//    const float delta_angle = atan2(delta.y, delta.x) - this->player.get_x_view_angle();
//    const float delta_angle = -atan2(delta.y, delta.x) + atan2(current_position.y - new_position.y, current_position.x - new_position.x);
//    const float angle_sin = sin(delta_angle/4);
    if(!new_block.is_portal && current_block.is_portal)
    {
        const Block &portal_to = this->world.get_block(current_block.portal_to_block_id);
//        translated_position = glm::vec3(new_position.x - current_block.world_position.x*64 + portal_to.world_position.x*64,
//                                        new_position.y - current_block.world_position.y*64 + portal_to.world_position.y*64,
//                                        new_position.z);
        translated_position = glm::vec3(new_position.x - current_block.world_position.x*(int)block_size + portal_to.world_position.x*(int)block_size,
                                        new_position.y - current_block.world_position.y*(int)block_size + portal_to.world_position.y*(int)block_size,
                                        new_position.z);
    }
    // Going forward through portal.
//    if((angle_sin <= 0.8f && angle_sin >= 0.8f) && !new_block.is_portal && current_block.is_portal)
//    {
//        const Block &portal_to = this->world.get_block(current_block.portal_to_block_id);
//
//        translated_position = glm::vec3(new_position.x - current_block.world_position.x*64 + portal_to.world_position.x*64,
//                                        new_position.y - current_block.world_position.y*64 + portal_to.world_position.y*64,
//                                        new_position.z);
//    }
    // Going backwards through portal.
//    else
//    if((angle_sin <= 0.8f && angle_sin >= 0.8f) && new_block.is_portal && !current_block.is_portal)
//    {
//        const Block &portal_to = this->world.get_block(new_block.portal_to_block_id);
//
//        translated_position = glm::vec3(new_position.x - new_block.world_position.x*64 + portal_to.world_position.x*64,
//                                        new_position.y - new_block.world_position.y*64 + portal_to.world_position.y*64,
//                                        new_position.z);
//    }
    return translated_position;
}


const glm::vec3 Physics::move(const glm::vec3 &current_position, const glm::vec3 &velocity) const
{

    float min_distance = INFINITY;
    int min_index = 0;

    this->rays[direction_right] = this->raycaster.raycast(right, current_position);
    this->rays[direction_up] = this->raycaster.raycast(up, current_position);
    this->rays[direction_left] = this->raycaster.raycast(left, current_position);
    this->rays[direction_down] = this->raycaster.raycast(down, current_position);
    glm::vec3 new_position = this->handle_wall_colisions(current_position, velocity);

    return handle_portals(current_position, new_position);
}
