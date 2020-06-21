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

enum Render_thread_task_type
{
    stop_thread_task, general_thread_task
};

class Producer_queue
{
private:
    std::mutex access_mutex;
    std::mutex lock_mutex;
    std::queue<std::pair<Render_thread_task_type, std::pair<glm::ivec2, glm::ivec2>>> task_queue;
public:
//    bool has_new_tasks() const
//    {
//        return task_queue.size() != 0;
//    }

    std::pair<Render_thread_task_type, std::pair<glm::ivec2, glm::ivec2>> get_task()
    {
        this->lock_mutex.lock();
        this->lock_mutex.unlock();
        this->access_mutex.lock();
        auto task = this->task_queue.front();
        this->access_mutex.unlock();
        return task;
    }
    void lock()
    {
        this->lock_mutex.lock();
    }

    void unlock()
    {
        this->lock_mutex.unlock();
    }
};

class Render_thread
{
private:
    World *world;
    Raycaster *raycaster;
    const Player *player;
    Sdl_wrapper *sdl_wrapper;
    const Texture_holder *texture_holder;
    Lookup_table *lookup;
    Framebuffer *framebuffer;

    std::queue<std::pair<glm::ivec2, glm::ivec2>> *tasks;
    std::mutex *queue_mutex;
    std::mutex *wait_mutex;

    glm::ivec2 angle_interval;
    glm::ivec2 column_interval;
    std::atomic_bool is_running;
    uint total_workers;
    int wall_height = 128*8;

    float get_block_x_uv(const Ray &ray, const Block &block);
    void draw_floor(const int &column, const int &row, const int &cast_degree, int height);
    void draw_ceiling(const int &column, const int &row, const int &cast_degree);
    void render_column(int cast_degree, const int &cast_column);
    void render_column2(int cast_degree, const int &cast_column);
    void render_variable_height(int cast_degree, const int &cast_column);
    void render();

public:
    Render_thread(World *world, Raycaster *raycaster, const Player *player,
                  Sdl_wrapper *sdl_wrapper, Texture_holder *texture_holder,
                  Lookup_table *lookup, Framebuffer *framebuffer);
    Render_thread(const Render_thread &other);
    void setup(std::queue<std::pair<glm::ivec2, glm::ivec2>> *tasks, std::mutex *queue_mutex,
               std::mutex *wait_mutex);
    void run();
    void stop();
};

#endif // RENDER_THREAD_H
