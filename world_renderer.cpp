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
    tasks_left.store(this->tasks.size());
    this->queue_mutex.unlock();
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
        Render_thread worker(&this->world, &this->raycaster, &this->player, &this->sdl_wrapper,
                             &this->texture_holder, &this->lookup, &this->framebuffer);
        worker.setup(&this->tasks, &this->queue_mutex, &this->new_task_mutex,
                     &this->new_task_cv, &this->task_done_cv, &this->tasks_left);
        this->workers.push_back(worker);
        this->threads.push_back(std::thread(&Render_thread::run, &this->workers.front()));
    }
}

World_renderer::~World_renderer()
{
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "~World_renderer: stopping threads.");
    for(auto &worker : this->workers)
        worker.stop();
    this->new_task_cv.notify_all();
    for(auto &thread : this->threads)
        thread.join();
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "~World_renderer: threads stopped succesfully.");
}

void World_renderer::render()
{
//    std::unique_lock<std::mutex>(this->new_task_mutex);
//    std::cout << "Generating tasks\n";
    this->generate_tasks();
    this->new_task_cv.notify_all();
    auto condition = [this](){return this->tasks_left.load() == 0;};
    std::unique_lock<std::mutex> task_done_lock(this->task_done_mutex);
    this->task_done_cv.wait(task_done_lock, condition);
//    std::cout << "Done rendering\n";
}
