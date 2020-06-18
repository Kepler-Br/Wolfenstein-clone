#ifndef RENDERER_H
#define RENDERER_H

#include "world.h"
#include "sdl_wrapper.h"
#include "player.h"
#include "raycaster.h"
#include "texture_holder.h"
#include "render_thread.h"
#include "framebuffer.h"

#include <condition_variable>
#include <atomic>
#include <thread>
#include <queue>

class World_renderer
{
    World &world;
    Sdl_wrapper &sdl_wrapper;
    const Player &player;
    Raycaster &raycaster;
    Lookup_table &lookup;
    Texture_holder &texture_holder;
    Framebuffer &framebuffer;

    std::queue<std::pair<glm::ivec2, glm::ivec2>> tasks;
    std::mutex queue_mutex;
    std::vector<Render_thread> workers;
    std::vector<std::thread> threads;
    std::vector<std::mutex *> wait_task_mutexes;
    uint render_cores;

    void generate_tasks();
    void lock_mutexes();
    void unlock_mutexes();
    void render_player(const glm::vec2 &center, const float &size);
    void render_blocks(const glm::vec2 &center, const float &size, const bool fill_screen);
    void render_cast_one(const glm::vec2 &center, const float &size);
public:
    World_renderer(World &world, Sdl_wrapper &sdl_wrapper, const Player &player,
                   Raycaster &raycaster, Lookup_table &lookup, Texture_holder &texture_holder,
                   Framebuffer &framebuffer, const uint &render_cores);
    ~World_renderer();

    void render();
    void render_map(const bool render_rays, const bool fill_screen, const float size);
};

#endif // RENDERER_H
