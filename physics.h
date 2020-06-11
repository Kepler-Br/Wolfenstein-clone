//
// Created by kepler-br on 6/7/20.
//

#ifndef WOLFENSHETIN_PHYSICS_H
#define WOLFENSHETIN_PHYSICS_H

#include "world.h"
#include "raycaster.h"
#include "player.h"
#include <glm/glm.hpp>

class Physics {
private:
    const World &world;
    const Raycaster &raycaster;
    const Player &player;
    static const int ray_count = 4;
    constexpr static float right = 0.0f;
    constexpr static float up = M_PI/2.0f;
    constexpr static float left = M_PI;
    constexpr static float down = 3.0f*M_PI/2.0f;
    Ray *rays;

    const glm::vec3 handle_wall_colisions(const glm::vec3 &current_position, const glm::vec3 &velocity) const;
    const glm::vec3 handle_portals(const glm::vec3 &current_position, const glm::vec3 &new_position) const;
public:
    Physics(const World& world, const Raycaster &raycaster, const Player &player);
    ~Physics();
    const glm::vec3 move(const glm::vec3 &current_position, const glm::vec3 &velocity) const;
};


#endif //WOLFENSHETIN_PHYSICS_H
