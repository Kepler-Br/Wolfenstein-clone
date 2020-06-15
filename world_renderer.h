#ifndef RENDERER_H
#define RENDERER_H

#include "world.h"
#include "sdl_wrapper.h"
#include "player.h"
#include "raycaster.h"
#include "texture_holder.h"
#include "render_thread.h"

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

    std::queue<std::pair<glm::ivec2, glm::ivec2>> tasks;
    std::condition_variable new_task_cv;
    std::condition_variable task_done_cv;
    std::mutex queue_mutex;
    std::mutex task_done_mutex;
    std::mutex new_task_mutex;
    std::vector<Render_thread> workers;
    std::vector<std::thread> threads;
    std::atomic<uint> tasks_left;
    uint render_cores;

    void generate_tasks();
public:
    World_renderer(World &world, Sdl_wrapper &sdl_wrapper, const Player &player,
             Raycaster &raycaster, Lookup_table &lookup, Texture_holder &texture_holder,
             const uint &render_cores);
    ~World_renderer();

    void render();
};

#endif // RENDERER_H
