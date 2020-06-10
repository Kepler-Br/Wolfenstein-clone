//
// Created by kepler-br on 6/6/20.
//

#ifndef WOLFENSHETIN_WORLD_RENDERER_H
#define WOLFENSHETIN_WORLD_RENDERER_H

#include "raycaster.h"
#include "world.h"
#include "player.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL_image.h>

class World_renderer {
private:
    World &world;
    const Raycaster &raycaster;
    const Player &player;
    Sdl_wrapper &sdl_wrapper;
    float field_of_view = 70.0f*M_PI/180.0f;
    int blockiness = 1;

    void render_view_rays(const glm::vec2 &center, const float &size);
    void render_one_block_view_rays(const glm::vec2 &center, const float &size);
    void render_blocks(const glm::vec2 &center, const float &size, const bool fill_screen);
    void render_player(const glm::vec2 &center, const float &size);

public:
    World_renderer(World &world, const Raycaster &raycaster, const Player &player, Sdl_wrapper &sdl_wrapper);
    void render();
    void render_map(const bool render_rays, const bool fill_screen, const float size = 1.0f);

    void set_fov(const float &fov);
    void set_blockiness(const int &blockiness);
};


#endif //WOLFENSHETIN_WORLD_RENDERER_H
