//
// Created by kepler-br on 6/5/20.
//

#ifndef WOLFENSHETIN_WORLD_H
#define WOLFENSHETIN_WORLD_H

#include <glm/glm.hpp>
#include "texture.h"

struct Block {
    glm::ivec2 world_position;
    glm::vec2 scaled_world_position;
    glm::ivec3 color;
    bool is_solid_wall;
    bool is_transparent;
    bool is_passable;
    bool is_portal;
    bool is_lit_by_lamp;
    bool seen_by_player;
    int portal_to_block_id;
    int right_texture_id;
    int up_texture_id;
    int left_texture_id;
    int down_texture_id;
    int floor_texture_id;
    int ceiling_texture_id;
};

class World {
private:
    Block *world;
    int world_length;
    glm::ivec2 world_dimensions;
    float block_size = int(64.0f/1.5f);

public:
    World();
    const int &get_world_length() const;
    const Block &get_block(const int index) const;
    Block &get_block(const int index);
    const Block &get_block(const glm::ivec2 &position) const;
    Block &get_block(const glm::ivec2 &position);
    const glm::ivec2 &get_world_dimensions() const;
    const float &get_block_size() const;
    void set_block_size(const float &block_size);
    void set_block_seen(const bool seen, const int index);
};


#endif //WOLFENSHETIN_WORLD_H
