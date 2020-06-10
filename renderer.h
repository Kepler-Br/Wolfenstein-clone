//
// Created by kepler-br on 6/6/20.
//

#ifndef WOLFENSHETIN_RENDERER_H
#define WOLFENSHETIN_RENDERER_H

#include "raycaster.h"
#include "world.h"
#include "player.h"
#include "sdl_instance.h"

class Renderer {
private:
    World &world;
    const Raycaster &raycaster;
    const Player &player;
    const Sdl_instance &sdl_instance;
    float field_of_view = 70.0f*M_PI/180.0f;
    int blockiness = 1;

    void render_view_rays(const glm::vec2 &center, const float &size);
    void render_one_block_view_rays(const glm::vec2 &center, const float &size);
    void render_blocks(const glm::vec2 &center, const float &size, const bool fill_screen);
    void render_player(const glm::vec2 &center, const float &size);

public:
    Renderer(World &world, const Raycaster &raycaster, const Player &player, const Sdl_instance &sdl_instance);
    void render_world();
    void render_map(const bool render_rays, const bool fill_screen, const float size = 1.0f);

    void set_fov(const float &fov);
    void set_blockiness(const int &blockiness);
};


#endif //WOLFENSHETIN_RENDERER_H
