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

void World_renderer::render_player(const glm::vec2 &center, const float &size)
{
    SDL_Renderer *renderer = this->sdl_wrapper.get_renderer();
    const float forward_length = 300.0f;
    const float square_size = 50.0f;
    const auto &forward = glm::vec2(this->lookup.cos(this->player.get_x_view_angle()),
                                    this->lookup.sin(this->player.get_x_view_angle()));
    SDL_Rect rect;

    SDL_SetRenderDrawColor(renderer, 237, 217, 154, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, center.x, center.y, center.x-forward.x*forward_length*size, center.y-forward.y*forward_length*size);

    rect = {int(center.x-size*square_size), int(center.y-size*square_size),
            int(size*square_size*2.0f), int(size*square_size*2.0f)};
    SDL_SetRenderDrawColor(renderer, 225, 65, 69, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
}

void World_renderer::render_blocks(const glm::vec2 &center, const float &size, const bool fill_screen)
{
    auto *renderer = this->sdl_wrapper.get_renderer();
    SDL_Rect rect;
    const float &block_size = this->world.get_block_size();

    SDL_SetRenderDrawColor(renderer, 59, 212, 145, SDL_ALPHA_OPAQUE);
    for (size_t i = 0; i < this->world.get_world_length(); i++)
    {
        const Block &block = this->world.get_block(i);
//        if(!block.is_solid_wall || !block.seen_by_player)
//            continue;
//        if(!block.is_solid_wall)
//            continue;
        const glm::vec2 &position = block.scaled_world_position*size;
        const glm::vec2 &player_relative = glm::vec2(this->player.get_position()*size) - position;
        rect = {int(player_relative.x-block_size*size+center.x), int(player_relative.y-block_size*size+center.y),
                (int)(block_size*size), (int)(block_size*size)};
        if (fill_screen)
            SDL_RenderFillRect(renderer, &rect);
        else
            SDL_RenderDrawRect(renderer, &rect);
    }
}

void World_renderer::render_cast_one(const glm::vec2 &center, const float &size)
{
    SDL_Renderer *renderer = this->sdl_wrapper.get_renderer();
    const float start_angle = this->player.get_x_view_angle() - this->lookup.angle30;
    const float end_angle = this->player.get_x_view_angle() + this->lookup.angle30;
    const float step = 1;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for(float xangle = start_angle; xangle < end_angle; xangle += step)
    {
        Ray raycast = this->raycaster.cast_one(this->player.get_position(), xangle);
                Block block = this->world.get_block(raycast.block_id);

        bool is_exeeding_world_dimensions = block.world_position.x > this->world.get_world_dimensions().x ||
                                            block.world_position.x == 0 ||
                                            block.world_position.y > this->world.get_world_dimensions().y ||
                                            block.world_position.y == 0;
        int total_length = raycast.length;
        while(!is_exeeding_world_dimensions)
        {
            glm::vec2 relative = glm::vec2(this->player.get_position()*size) - glm::vec2(raycast.position.x * size, raycast.position.y * size);
            SDL_RenderDrawLine(renderer, center.x, center.y,
                            relative.x + center.x, relative.y + center.y);
            raycast = this->raycaster.cast_one(raycast.position, xangle);
            block = this->world.get_block(raycast.block_id);
            total_length += raycast.length;
            is_exeeding_world_dimensions = block.world_position.x > this->world.get_world_dimensions().x ||
                                                        block.world_position.x == 0 ||
                                                        block.world_position.y > this->world.get_world_dimensions().y ||
                                                        block.world_position.y == 0;
        }
    }
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

void World_renderer::render_map(const bool render_rays, const bool fill_screen, const float size)
{
    const auto &renderer = this->sdl_wrapper.get_renderer();
    const glm::ivec2 center = {
        this->sdl_wrapper.get_resolution().x / 2.0f,
        this->sdl_wrapper.get_resolution().y / 2.0f};
    SDL_Rect rect;

    if(fill_screen)
    {
        rect = {0, 0,
                this->sdl_wrapper.get_resolution().x,
                this->sdl_wrapper.get_resolution().y};
        SDL_SetRenderDrawColor(renderer, 59, 114, 123, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
    }

//    if(render_rays)
//        this->render_one_block_view_rays(center, size);
    this->render_blocks(center, size, fill_screen);
    this->render_cast_one(center, size);
    this->render_player(center, size);
}
