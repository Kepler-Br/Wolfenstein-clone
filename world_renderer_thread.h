//
// Created by kepler-br on 6/10/20.
//

#ifndef WOLFENSHETIN_WORLD_RENDERER_THREAD_H
#define WOLFENSHETIN_WORLD_RENDERER_THREAD_H

#include <mutex>
#include <condition_variable>
#include <queue>

class World_renderer_thread
{
    std::mutex *renderer_mutex;
    std::mutex *tast_wait_mutex;
    World *world;
    const Raycaster *raycaster;
    const Player *player;
    Sdl_wrapper *sdl_wrapper;
    std::condition_variable *new_tasks;
    std::queue<std::pair<glm::vec2, glm::ivec2>> *tasks;
    bool stopping = false;
//    std::mutex test;

public:
    World_renderer_thread() = default;
    World_renderer_thread(std::queue<std::pair<glm::vec2, glm::ivec2>> *tasks, std::condition_variable *new_tasks, std::mutex *tast_wait_mutex, std::mutex *renderer_mutex, World *world, const Raycaster *raycaster, const Player *player, Sdl_wrapper *sdl_wrapper):
    new_tasks(new_tasks),
    tast_wait_mutex(tast_wait_mutex),
    renderer_mutex(renderer_mutex),
    world(world),
    raycaster(raycaster),
    player(player),
    sdl_wrapper(sdl_wrapper),
    tasks(tasks)
    {

    }
    void stop()
    {
        this->stopping = true;
    }
    void operator()()
    {
        this->stopping = false;

        while(!this->stopping)
        {
            std::unique_lock<std::mutex> lock(*tast_wait_mutex);
            new_tasks->wait(lock, [this]{return !this->tasks->empty() || this->stopping;});
            if(this->stopping)
                break;
            glm::ivec2 x_range;
            glm::vec2 angle_range;
            this->renderer_mutex->lock();
            std::tie(x_range, angle_range) = this->tasks->back();
            this->tasks->pop();
            this->renderer_mutex->unlock();
            std::cout << "Processing data (" << x_range.x << ", " << x_range.y << ")\n";
        }
    }
};

#endif //WOLFENSHETIN_WORLD_RENDERER_THREAD_H
