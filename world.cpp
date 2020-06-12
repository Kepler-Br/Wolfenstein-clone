#include "world.h"
#include <cmath> // DELETE ME
#include <stdexcept>

World::World() {
    this->world_dimensions = {12, 12};
    this->world_length = world_dimensions.x * world_dimensions.y;
//    int *blocks = new int[this->world_length] {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//                                               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int *blocks = new int[this->world_length] {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    this->world = new Block[this->world_length];

    for (size_t i = 0; i < this->world_length; i++)
    {
        const glm::vec2 world_position = {i % this->world_dimensions.x, i / this->world_dimensions.y};
        const glm::vec2 scaled_world_position = glm::vec2(world_position.x*this->block_size, world_position.y*this->block_size);
        this->world[i] = {world_position,
                          scaled_world_position,
                          glm::ivec3(255, 255, 255),
                          true, false, true, false, false, false, false, false,
                          0, 0, 0, {0, 0, 0, 0}, 0, 0};
        if (blocks[i] == 0)
            this->world[i].is_solid_wall = false;
        else if (blocks[i] == 1)
        {
            this->world[i].is_solid_wall = true;
            this->world[i].is_passable = false;
        }
//        else if (blocks[i] == 2)
//        {
//            this->world[i].is_solid_wall = true;
//            this->world[i].is_portal = true;
//        }
        this->world[i].color = {rand() % 255, rand() % 255, rand() % 255};
    }
    this->world[60].wall_textures.up = 1;
    delete[] blocks;
}

const glm::ivec2 &World::get_world_dimensions() const
{
    return this->world_dimensions;
}

const Block &World::get_block(const glm::ivec2 &position) const
{
    if (position.x >= this->world_dimensions.x || position.y >= this->world_dimensions.y || position.x < 0 || position.y < 0)
        throw std::out_of_range("Tried to access index outside of the map.");
//        return this->world[0];
    const int index = position.y * this->world_dimensions.x + position.x;
    return this->world[index];
}

Block &World::get_block(const glm::ivec2 &position)
{
    if (position.x >= this->world_dimensions.x || position.y >= this->world_dimensions.y || position.x < 0 || position.y < 0)
//        return this->world[0];
        throw std::out_of_range("Tried to access index outside of the map.");
    const int index = position.y * this->world_dimensions.x + position.x;
    return this->world[index];
}

const Block &World::get_block(const size_t &index) const
{
    if (index >= this->world_length || index < 0)
        throw std::out_of_range("Tried to access index outside of the map.");
//        return this->world[0];
    return this->world[index];
}

Block &World::get_block(const size_t &index)
{
    if (index >= this->world_length || index < 0)
        throw std::out_of_range("Tried to access index outside of the map.");
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
