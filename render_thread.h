#ifndef RENDER_THREAD_H
#define RENDER_THREAD_H

#include "player.h"
#include "raycaster.h"
#include "world.h"
#include "sdl_wrapper.h"
#include "framebuffer.h"
#include "texture_holder.h"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <queue>

class Render_thread
{
private:
    World &world;
    Raycaster &raycaster;
    const Player &player;
    Sdl_wrapper &sdl_wrapper;
    const Texture_holder &texture_holder;
    Lookup_table &lookup;
    Framebuffer &framebuffer;

    std::queue<std::pair<glm::ivec2, glm::ivec2>> *tasks;
    std::condition_variable *new_task_cv;
    std::condition_variable *task_done_cv;
    std::mutex *queue_mutex;
    std::mutex *new_task_mutex;
    std::atomic<uint> *tasks_left;

    glm::ivec2 angle_interval;
    glm::ivec2 column_interval;
    std::atomic_bool is_running;
    uint total_workers;
    int wall_height = 128*8;

    float get_block_x_uv(const Ray &ray, const Block &block);
    void draw_floor(const int &column, const int &row, const int &cast_degree);
    void draw_ceiling(const int &column, const int &row, const int &cast_degree);
    void render_column(int cast_degree, const int &cast_column);
    void render();

public:
    Render_thread(World &world, Raycaster &raycaster, const Player &player,
                  Sdl_wrapper &sdl_wrapper, Texture_holder &texture_holder,
                  Lookup_table &lookup, Framebuffer &framebuffer);
    Render_thread(const Render_thread &other);
    void setup(std::queue<std::pair<glm::ivec2, glm::ivec2>> *tasks, std::mutex *queue_mutex,
               std::mutex *new_task_mutex, std::condition_variable *new_task_cv, std::condition_variable *task_done_cv, std::atomic<uint> *tasks_left);
    void run();
    void stop();
};

#endif // RENDER_THREAD_H
