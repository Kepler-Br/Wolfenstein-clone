//
// Created by kepler-br on 6/5/20.
//

#ifndef WOLFENSHETIN_RAYCASTER_H
#define WOLFENSHETIN_RAYCASTER_H
#include <glm/glm.hpp>
#include "world.h"

struct Ray {
    int block_index;
    glm::vec2 ray_position;
    float ray_length;
};

enum block_side {
    right = 0,
    up,
    left,
    down
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
    const block_side get_block_side(const glm::vec2 &position) const
    {
        return right;
    }
};


#endif //WOLFENSHETIN_RAYCASTER_H
