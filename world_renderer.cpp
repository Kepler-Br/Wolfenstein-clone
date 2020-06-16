#include "world_renderer.h"
#include <iostream> //DELETEME

void World_renderer::generate_tasks()
{

    const int x_resolution = this->framebuffer.get_resolution().x;
    int step = x_resolution/this->render_cores;
    int angle = this->player.get_x_view_angle() - this->lookup.angle30;
    if (angle < 0)
        angle=this->lookup.angle360 + angle;
    this->queue_mutex.lock();
    for(int x = step, prev_x = 0; x <= x_resolution; prev_x = x, x += step)
    {
        this->tasks.push(std::make_pair(glm::ivec2(angle, angle+step), glm::ivec2(prev_x, x)));
        angle+=step;
    }
    this->queue_mutex.unlock();
}

void World_renderer::lock_mutexes()
{
    for(auto &mutex : this->wait_task_mutexes)
        mutex->lock();
}

void World_renderer::unlock_mutexes()
{
    for(auto &mutex : this->wait_task_mutexes)
        mutex->unlock();
}

World_renderer::World_renderer(World &world, Sdl_wrapper &sdl_wrapper, const Player &player,
                   Raycaster &raycaster, Lookup_table &lookup,
                   Texture_holder &texture_holder, Framebuffer &framebuffer, const uint &render_cores = 4):
    world(world),
    sdl_wrapper(sdl_wrapper),
    player(player),
    raycaster(raycaster),
    lookup(lookup),
    texture_holder(texture_holder),
    framebuffer(framebuffer)
{
    this->workers.reserve(render_cores);
    this->render_cores = render_cores;
    for(uint i = 0; i < render_cores; i++)
    {
        this->wait_task_mutexes.push_back(new std::mutex());
        Render_thread worker(&this->world, &this->raycaster, &this->player, &this->sdl_wrapper,
                             &this->texture_holder, &this->lookup, &this->framebuffer);
        worker.setup(&this->tasks, &this->queue_mutex, this->wait_task_mutexes[i]);
        this->workers.push_back(worker);
        this->threads.push_back(std::thread(&Render_thread::run, &this->workers[i]));
    }
}

World_renderer::~World_renderer()
{
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "~World_renderer: stopping threads.");
    for(auto &worker : this->workers)
        worker.stop();
    this->unlock_mutexes();
    for(auto &thread : this->threads)
        thread.join();
    for(auto *mutex : this->wait_task_mutexes)
        delete mutex;
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "~World_renderer: threads stopped succesfully.");
}

void World_renderer::render()
{
    this->generate_tasks();
//    std::cout << this->tasks.size() << std::endl;
    this->unlock_mutexes();
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    this->lock_mutexes();
//    std::cout << "Done rendering\n";
}
