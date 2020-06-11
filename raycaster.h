//
// Created by kepler-br on 6/5/20.
//

#ifndef WOLFENSHETIN_RAYCASTER_H
#define WOLFENSHETIN_RAYCASTER_H
#include <glm/glm.hpp>
#include "world.h"
#include "types.h"

struct Ray {
    int block_index;
    glm::vec2 ray_position;
    float ray_length;
    directions hit_side;
};

class Raycaster {
private:
    const World &world;

public:
    explicit Raycaster(const World &world);
    const Ray raycast(float ray_angle, const glm::vec2 &start_position) const;
    const Ray raycast(const glm::vec2 &ray_direction, const glm::vec2 &start_position) const;
    const Ray raycast_one(float ray_angle, const glm::vec2 &start_position) const;
    const Ray raycast_one(const glm::vec2 &ray_direction, const glm::vec2 &start_position) const;
//    const directions get_block_side(glm::vec2 position) const
//    {
//        if (position.x >= this->world.get_block_size())
//            position.x -= this->world.get_block_size()*std::trunc(position.x/this->world.get_block_size());
//        if (position.y >= this->world.get_block_size())
//            position.y -= this->world.get_block_size()*std::trunc(position.y/this->world.get_block_size());
//
//        return direction_up;
//    }
};


#endif //WOLFENSHETIN_RAYCASTER_H
