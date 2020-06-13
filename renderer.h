#ifndef RENDERER_H
#define RENDERER_H

#include "world.h"
#include "sdl_wrapper.h"
#include "player.h"
#include "raycaster.h"

class Renderer
{
    const World &world;
    Sdl_wrapper &sdl_wrapper;
    const Player &player;
    Raycaster &raycaster;
    Lookup_table &lookup;
    int wall_height = 64;

    void render_player(const size_t &block_size);
public:
    Renderer(const World &world, Sdl_wrapper &sdl_wrapper, const Player &player, Raycaster &raycaster, Lookup_table &lookup);

    void draw_map();
    void draw_world();
};

#endif // RENDERER_H
