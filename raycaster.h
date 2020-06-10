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

class Raycaster {
private:
    const World &world;

public:
    explicit Raycaster(const World &world);
    const Ray raycast(float ray_angle, const glm::vec2 &start_position) const;
    const Ray raycast(const glm::vec2 &ray_direction, const glm::vec2 &start_position) const;
    const Ray raycast_one(float ray_angle, const glm::vec2 &start_position) const;
    const Ray raycast_one(const glm::vec2 &ray_direction, const glm::vec2 &start_position) const;
};


#endif //WOLFENSHETIN_RAYCASTER_H
