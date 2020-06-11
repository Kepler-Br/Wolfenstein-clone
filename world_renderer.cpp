//
// Created by kepler-br on 6/6/20.
//

#include "world_renderer.h"

World_renderer::World_renderer(World &world, const Raycaster &raycaster, const Player &player, Sdl_wrapper &sdl_wrapper):
        world(world),
        raycaster(raycaster),
        player(player),
        sdl_wrapper(sdl_wrapper)
{
    const glm::ivec2 &resolution = this->sdl_wrapper.resolution;
//    this->threads = new std::thread[this->threads_total];
//    this->renderers = new World_renderer_thread[this->threads_total];
//    for(int i = 0; i < this->threads_total; i++)
//    {
//        this->renderers[i] = World_renderer_thread(&tasks, &this->new_tasks, &task_wait_mutex, &this->render_mutex, &world, &raycaster, &player, &sdl_wrapper);
//        this->threads[i] = std::thread(this->renderers[i]);
//    }
}

World_renderer::~World_renderer()
{
// `   std::cout << "Waiting for threads to stop\n";
//    for(int i = 0; i < this->threads_total; i++)
//    {
//        this->renderers->stop();
//        this->threads[i].join();
//        std::cout << "Thread stopped\n";
//    }
//
//    delete[] this->threads;
//    delete[] this->renderers;`
}


void World_renderer::render()
{
//    this->render_mutex.lock();
//    const float start_angle = this->player.get_x_view_angle() - this->field_of_view / 2.0f;
//    const float end_angle = this->player.get_x_view_angle() +this->field_of_view / 2.0f;
//    const float step = this->field_of_view/this->threads_total;
//    const int xstep = this->sdl_wrapper.resolution.x / this->threads_total;
//    int x = 0;
//    for(float xangle = start_angle; xangle < end_angle - step; xangle += step)
//    {
//        glm::vec2 xangle_interval = glm::vec2(xangle, xangle + step);
//        glm::ivec2 x_interval = glm::ivec2(x, x + xstep);
//        this->tasks.push(std::make_pair(xangle_interval, x_interval));
//        x += xstep;
//    }
//    this->render_mutex.unlock();
//    this->new_tasks.notify_all();
//    std::cout << "Pushed new tasks\n";

    const float &block_size = this->world.get_block_size();

    int x = 0;
    const float start_angle = this->player.get_x_view_angle() - this->field_of_view / 2.0f;
    const float end_angle = this->player.get_x_view_angle() + this->field_of_view / 2.0f;
    const float step = this->field_of_view / (this->sdl_wrapper.resolution.x / this->blockiness);
    this->sdl_wrapper.lock_framebuffer();
    this->sdl_wrapper.clear_framebuffer();
    for(float xangle = start_angle; xangle < end_angle; xangle += step)
    {
        const glm::ivec2 player_block_position = {int(this->player.get_position().x)/block_size, int(this->player.get_position().y)/block_size};
        const Block &player_block = this->world.get_block(player_block_position);
        Ray raycast;
        Block block;
        if(x >= this->sdl_wrapper.resolution.x)
        {
            x++;
            continue;
        }
        if(player_block.is_portal)
        {
            Block portal_to_block = this->world.get_block(player_block.portal_to_block_id);
            glm::vec2 new_position = glm::vec2(this->player.get_position()) - glm::vec2(player_block_position.x*block_size, player_block_position.y*block_size) + portal_to_block.scaled_world_position;
            raycast = this->raycaster.raycast(xangle, new_position);

            block = this->world.get_block(raycast.block_index);
        }
        else
        {
            raycast = this->raycaster.raycast(xangle, this->player.get_position());
            block = this->world.get_block(raycast.block_index);
            if(block.is_portal)
            {
                const float length = raycast.ray_length;
                Block portal_to_block = this->world.get_block(block.portal_to_block_id);
                glm::vec2 new_position = raycast.ray_position - block.scaled_world_position + portal_to_block.scaled_world_position;
                raycast = this->raycaster.raycast(xangle, new_position);
                raycast.ray_length += length;
                block = this->world.get_block(raycast.block_index);
            }
        }

        world.set_block_seen(true, raycast.block_index);
        float fish_eye_fix = this->player.get_x_view_angle() - xangle;
        if(fish_eye_fix < 0)
            fish_eye_fix += 2*M_PI;
        if (fish_eye_fix > 2*M_PI)
            fish_eye_fix -= 2*M_PI;
        float fixed_length = raycast.ray_length * std::cos(fish_eye_fix);
        float line_height = (124*this->sdl_wrapper.resolution.y) / fixed_length;
        float line_offset = this->sdl_wrapper.resolution.y / 3.0f - line_height / 2.0f + this->player.get_x_view_angle() + this->player.get_y_view_angle();
        Texture &brick_texture = this->world.brick_texture;
        Texture &xyu_texture = this->world.xyu_texture;

        float uv_start = 0.0f;
        float old_height = line_height;
        float uv_step = (std::abs(uv_start)+1.0f)/old_height;

        if (int(line_height + line_offset) >= this->sdl_wrapper.resolution.y)
        {
           line_height = line_height - (line_height + line_offset - this->sdl_wrapper.resolution.y) - 1;
        }


        if(line_offset < 0)
        {
            uv_start = (-line_offset)/old_height;
            line_height += line_offset;
            line_offset = 0;
        }

        glm::vec2 uv = glm::vec2((raycast.ray_position.x - block.scaled_world_position.x) / this->world.get_block_size(), uv_start);
        uv.x = this->get_block_x_uv(raycast, block);
        for(float i = 0; i < line_height; i++)
        {
            uv.y += uv_step;
            Pixel pixel;
            if(raycast.hit_side == direction_up)
                if(block.up_texture_id == 0)
                    pixel = brick_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
                else
                    pixel = xyu_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
            if(raycast.hit_side == direction_down)
                if(block.down_texture_id == 0)
                    pixel = brick_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
                else
                    pixel = xyu_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
            if(raycast.hit_side == direction_left)
                if(block.left_texture_id == 0)
                    pixel = brick_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
                else
                    pixel = xyu_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
            if(raycast.hit_side == direction_right)
                if(block.right_texture_id == 0)
                    pixel = brick_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
                else
                    pixel = xyu_texture.get_normalized_pixel(glm::vec2(uv.x, uv.y));
            this->sdl_wrapper.set_framebuffer_pixel(glm::ivec3(pixel.r, pixel.g, pixel.b), glm::ivec2(x, line_offset + i));
        }
        x++;
    }
    this->sdl_wrapper.unlock_framebuffer();
    this->sdl_wrapper.put_framebuffer();
}

void World_renderer::render_map(const bool render_rays, const bool fill_screen, const float size)
{
    const auto &renderer = this->sdl_wrapper.get_renderer();
    const auto &forward = this->player.get_forward();
    const glm::ivec2 center = {this->sdl_wrapper.resolution.x / 2.0f, this->sdl_wrapper.resolution.y / 2.0f};
    SDL_Rect rect;

    if(fill_screen)
    {
        rect = {0, 0, this->sdl_wrapper.resolution.x, this->sdl_wrapper.resolution.y};
        SDL_SetRenderDrawColor(renderer, 59, 114, 123, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
    }
    this->render_player(center, size);
    if(render_rays)
        this->render_one_block_view_rays(center, size);
    this->render_blocks(center, size, fill_screen);
}

void World_renderer::set_fov(const float &fov)
{
    const float max_fov = 360.0f;
    const float min_fov = 30.0f;

    if (fov > 360.0f)
        this->field_of_view = max_fov*M_PI/180.0f;
    else if (fov < min_fov)
        this->field_of_view = min_fov*M_PI/180.0f;
    else
        this->field_of_view = fov*M_PI/180.0f;
}

void World_renderer::set_blockiness(const int &blockiness) {
    const int max_blockiness = 20;

    if (blockiness < 1)
        this->blockiness = 1;
    else if (blockiness > max_blockiness)
        this->blockiness = max_blockiness;
    else
        this->blockiness = blockiness;
}

void World_renderer::render_view_rays(const glm::vec2 &center, const float &size)
{
    const auto &renderer = this->sdl_wrapper.get_renderer();
    const float start_angle = this->player.get_x_view_angle() - this->field_of_view / 2.0f;
    const float end_angle = this->player.get_x_view_angle() + this->field_of_view / 2.0f;
    const float step = this->field_of_view / (this->sdl_wrapper.resolution.x / this->blockiness);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for(float xangle = start_angle; xangle < end_angle; xangle += step)
    {
        Ray raycast = this->raycaster.raycast(xangle, this->player.get_position());
        Block block = this->world.get_block(raycast.block_index);
        if (block.is_portal)
        {
            glm::vec2 relative = glm::vec2(this->player.get_position()*size) - raycast.ray_position*size;
            SDL_RenderDrawLine(renderer, center.x, center.y,
                               relative.x + center.x, relative.y + center.y);
            Block portal_to_block = this->world.get_block(block.portal_to_block_id);
            glm::vec2 new_position = raycast.ray_position - block.scaled_world_position + portal_to_block.scaled_world_position;
            raycast = this->raycaster.raycast(xangle, new_position);
            block = this->world.get_block(raycast.block_index);
            relative = glm::vec2(this->player.get_position()*size) - raycast.ray_position*size;
            glm::vec2 relative2 = glm::vec2(this->player.get_position()*size) - new_position*size;
            SDL_RenderDrawLine(renderer, relative2.x + center.x, relative2.y + center.y,
                               relative.x + center.x, relative.y + center.y);
        }
        else
        {
            const glm::vec2 relative = glm::vec2(this->player.get_position()*size) - raycast.ray_position*size;
            SDL_RenderDrawLine(renderer, center.x, center.y,
                               relative.x + center.x, relative.y + center.y);
        }
    }
}

void World_renderer::render_player(const glm::vec2 &center, const float &size)
{
    SDL_Renderer *renderer = this->sdl_wrapper.get_renderer();
    const float forward_length = 20.0f;
    const float square_size = 3.0f;
    const auto &forward = this->player.get_forward();
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
    for (int i = 0; i < this->world.get_world_length(); i++)
    {
        const Block &block = this->world.get_block(i);
        if(!block.is_solid_wall || !block.seen_by_player)
            continue;
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

void World_renderer::render_one_block_view_rays(const glm::vec2 &center, const float &size)
{
    SDL_Renderer *renderer = this->sdl_wrapper.get_renderer();
    const float start_angle = this->player.get_x_view_angle() - this->field_of_view / 2.0f;
    const float end_angle = this->player.get_x_view_angle() + this->field_of_view / 2.0f;
    const float step = this->field_of_view / (this->sdl_wrapper.resolution.x / this->blockiness);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for(float xangle = start_angle; xangle < end_angle; xangle += step)
    {
        Ray raycast = this->raycaster.raycast(xangle, this->player.get_position());
        const glm::vec2 relative = glm::vec2(this->player.get_position()*size) - raycast.ray_position*size;
        SDL_RenderDrawLine(renderer, center.x, center.y,
                           relative.x + center.x, relative.y + center.y);
    }
}
