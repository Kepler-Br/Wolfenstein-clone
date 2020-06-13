#ifndef RENDERER_H
#define RENDERER_H

#include "world.h"
#include "sdl_wrapper.h"
#include "player.h"
#include "raycaster.h"
#include "texture_holder.h"

class Renderer
{
    const World &world;
    Sdl_wrapper &sdl_wrapper;
    const Player &player;
    Raycaster &raycaster;
    Lookup_table &lookup;
    Texture_holder &texture_holder;
    int wall_height = 64;

    void render_player(const size_t &block_size);
    float get_block_x_uv(const Ray &ray, const Block &block);
public:
    Renderer(const World &world, Sdl_wrapper &sdl_wrapper, const Player &player, Raycaster &raycaster, Lookup_table &lookup, Texture_holder &texture_holder);

    void draw_map();
    void draw_world();
};

#endif // RENDERER_H
