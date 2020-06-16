//
// Created by kepler-br on 6/12/20.
//

#ifndef WOLFENSTEIN_WORLD_H
#define WOLFENSTEIN_WORLD_H

#include "types.h"
#include <glm/vec2.hpp>

class World
{
private:
    Block *world;
    size_t world_length;
    glm::ivec2 world_dimensions;
    int block_size = 512;

public:
    World();
    const size_t &get_world_length() const;
    const Block &get_block(const size_t &index) const;
    Block &get_block(const size_t &index);
    const Block &get_block(const glm::ivec2 &position) const;
    Block &get_block(const glm::ivec2 &position);
    const glm::ivec2 &get_world_dimensions() const;
    const int &get_block_size() const;
    void set_block_size(const int &block_size);
    void set_block_seen(const bool seen, const size_t index);
    void bake_light();
};


#endif //WOLFENSTEIN_WORLD_H
