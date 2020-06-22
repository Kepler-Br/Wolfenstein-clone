#include "world.h"
#include <cmath> // DELETE ME
#include <stdexcept>
#include <iostream> // DELETE ME

World::World() {
//    this->world_dimensions = {12, 12};
    this->world_dimensions = {11, 11};
    this->world_length = world_dimensions.x * world_dimensions.y;
    int *blocks = new int[this->world_length] {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                               1, 0, 0, 0, 0, 5000, 5000, 5000, 0, 0, 1,
                                               1, 0, 0, 0, 0, 5000, 0, 0, 0, 0, 1,
                                               1, 0, 0, 0, 0, 5000, 5000, 5000, 0, 0, 1,
                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                               1, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1,
                                               1, 100, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                               10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
//    int *blocks = new int[this->world_length] {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    this->world = new Block[this->world_length];

    for (size_t i = 0; i < this->world_length; i++)
    {
        const glm::vec2 world_position = {i % this->world_dimensions.x, i / this->world_dimensions.y};
        const glm::vec2 scaled_world_position = glm::vec2(world_position.x*this->block_size, world_position.y*this->block_size);
//        const total_textures = 4;
        this->world[i] = {world_position,
                          scaled_world_position,
                          .color=glm::ivec3(255, 255, 255),
                          .is_solid_wall=true, .is_transparent=false, .is_passable=true, .is_portal=false,
                          .is_lit_by_lamp=false, .seen_by_player=false, .is_ceiling_sky=true, .is_bottom_sky=false,
                          .portal_to_block_id=0, .top_sky_texture_id=0, .bottom_sky_texture_id=0,
//                          {(size_t)rand()%5, (size_t)rand()%5, (size_t)rand()%5, (size_t)rand()%5},
                          {8,8,8,8},
                          .floor_texture_id=7,
                          .ceiling_texture_id=5,
                          .floor_height=/*(size_t)(this->block_size)+*/ 100 + (size_t)rand()%500/*(size_t)blocks[i]*/};
        this->world[i].is_solid_wall = false;
        this->world[i].floor_height = blocks[i];

        this->world[i].color = {rand() % 255, rand() % 255, rand() % 255};
    }
    this->world[60].wall_textures.right = 1;
    this->world[60].wall_textures.left = 2;
    delete[] blocks;
}

const glm::ivec2 &World::get_world_dimensions() const
{
    return this->world_dimensions;
}

const Block &World::get_block(const glm::ivec2 &position) const
{
    if (position.x >= this->world_dimensions.x || position.y >= this->world_dimensions.y || position.x < 0 || position.y < 0)
        return this->world[0];
//        throw std::out_of_range("Tried to access index outside of the map.");
    const int index = position.y * this->world_dimensions.x + position.x;
    return this->world[index];
}

Block &World::get_block(const glm::ivec2 &position)
{
    if (position.x >= this->world_dimensions.x || position.y >= this->world_dimensions.y || position.x < 0 || position.y < 0)
        return this->world[0];
//        throw std::out_of_range("Tried to access index outside of the map.");
    const int index = position.y * this->world_dimensions.x + position.x;
    return this->world[index];
}

const Block &World::get_block(const size_t &index) const
{
    if (index >= this->world_length)
        return this->world[0];
//        throw std::out_of_range("Tried to access index outside of the map.");
//        return this->world[0];
    return this->world[index];
}

Block &World::get_block(const size_t &index)
{
    if (index >= this->world_length)
        return this->world[0];
//        throw std::out_of_range("Tried to access index outside of the map.");
    return this->world[index];
}

const size_t &World::get_world_length() const
{
    return this->world_length;
}

const int &World::get_block_size() const
{
    return this->block_size;
}

void World::set_block_seen(const bool seen, const size_t index)
{
    this->get_block(index).seen_by_player = seen;
}

void World::set_block_size(const int &block_size)
{
    this->block_size = block_size;
}
