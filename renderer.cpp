//
// Created by kepler-br on 6/6/20.
//

#include "renderer.h"

Renderer::Renderer(World &world, const Raycaster &raycaster, const Player &player, const Sdl_instance &sdl_instance):
    world(world),
    raycaster(raycaster),
    player(player),
    sdl_instance(sdl_instance)
{

}

void Renderer::render_world()
{
    const auto &renderer = this->sdl_instance.renderer;
    const float &block_size = this->world.get_block_size();

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    int x = 0;
    const float start_angle = this->player.get_view_angle() - this->field_of_view / 2.0f;
    const float end_angle = this->player.get_view_angle() +this->field_of_view / 2.0f;
    const float step = this->field_of_view / (this->sdl_instance.resolution.x/this->blockiness);
    for(float xangle = start_angle; xangle < end_angle; xangle += step)
    {
//      const glm::ivec2 player_block_position = {int(this->player.get_position().x)/64, int(this->player.get_position().y)/64};
        const glm::ivec2 player_block_position = {int(this->player.get_position().x)/block_size, int(this->player.get_position().y)/block_size};
        const Block &player_block = this->world.get_block(player_block_position);
        Ray raycast;
        Block block;
        if(player_block.is_portal)
        {
            Block portal_to_block = this->world.get_block(player_block.portal_to_block_id);
//            glm::vec2 new_position = glm::vec2(this->player.get_position()) - glm::vec2(player_block_position*64) + portal_to_block.scaled_world_position;
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
//        if (!player_block.is_portal && block.is_portal)
//        {
//            const float length = raycast.ray_length;
//            Block portal_to_block = this->world.get_block(block.portal_to_block_id);
//            glm::vec2 new_position = raycast.ray_position - block.scaled_world_position + portal_to_block.scaled_world_position;
//            raycast = this->raycaster.raycast(xangle, new_position);
//            raycast.ray_length += length;
//            block = this->world.get_block(raycast.block_index);
//        }
        world.set_block_seen(true, raycast.block_index);
        float fish_eye_fix = this->player.get_view_angle() - xangle;
        if(fish_eye_fix < 0)
            fish_eye_fix += 2*M_PI;
        if (fish_eye_fix > 2*M_PI)
            fish_eye_fix -= 2*M_PI;
        float fixed_length = raycast.ray_length * std::cos(fish_eye_fix);
        float line_height = (64*this->sdl_instance.resolution.y)/fixed_length;
        float line_offset = this->sdl_instance.resolution.y/3.0f-line_height/2.0f;

        if (line_height + line_offset > this->sdl_instance.resolution.y)
            line_height = line_height - (line_height + line_offset - this->sdl_instance.resolution.y);
        if (line_height + line_offset > this->sdl_instance.resolution.y)
            std::cout << line_height + line_offset << std::endl;
        {
            for(float i = 0; i < line_height; i++)
            {
                SDL_SetRenderDrawColor(renderer, block.color.x, block.color.y, block.color.z, 255);
                SDL_RenderDrawPoint(renderer, x, line_offset + i);
            }
        }

//        for (int blockiness = 0; blockiness < this->blockiness; blockiness++)
//        {
//            SDL_SetRenderDrawColor(renderer, block.color.x, block.color.y, block.color.z, 255);
//            SDL_RenderDrawLine(renderer, x*this->blockiness+blockiness, 0+line_offset, x*this->blockiness+blockiness, line_height+line_offset);
//        }
        x++;
    }
}

void Renderer::render_map(const bool render_rays, const bool fill_screen, const float size)
{
    const auto &renderer = this->sdl_instance.renderer;
    const auto &forward = this->player.get_forward();
    const glm::ivec2 center = {this->sdl_instance.resolution.x/2.0f, this->sdl_instance.resolution.y/2.0f};
    SDL_Rect rect;

    if(fill_screen)
    {
        rect = {0, 0, this->sdl_instance.resolution.x, this->sdl_instance.resolution.y};
        SDL_SetRenderDrawColor(renderer, 59, 114, 123, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
    }
    this->render_player(center, size);
    if(render_rays)
        this->render_one_block_view_rays(center, size);
    this->render_blocks(center, size, fill_screen);
}

void Renderer::set_fov(const float &fov)
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

void Renderer::set_blockiness(const int &blockiness) {
    const int max_blockiness = 20;

    if (blockiness < 1)
        this->blockiness = 1;
    else if (blockiness > max_blockiness)
        this->blockiness = max_blockiness;
    else
        this->blockiness = blockiness;
}

void Renderer::render_view_rays(const glm::vec2 &center, const float &size)
{
    const auto &renderer = this->sdl_instance.renderer;
    const float start_angle = this->player.get_view_angle() - this->field_of_view / 2.0f;
    const float end_angle = this->player.get_view_angle() +this->field_of_view / 2.0f;
    const float step = this->field_of_view / (this->sdl_instance.resolution.x/this->blockiness);

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

void Renderer::render_player(const glm::vec2 &center, const float &size)
{
    const auto &renderer = this->sdl_instance.renderer;
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

void Renderer::render_blocks(const glm::vec2 &center, const float &size, const bool fill_screen)
{
    const auto &renderer = this->sdl_instance.renderer;
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

void Renderer::render_one_block_view_rays(const glm::vec2 &center, const float &size)
{
    const auto &renderer = this->sdl_instance.renderer;
    const float start_angle = this->player.get_view_angle() - this->field_of_view / 2.0f;
    const float end_angle = this->player.get_view_angle() +this->field_of_view / 2.0f;
    const float step = this->field_of_view / (this->sdl_instance.resolution.x/this->blockiness);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for(float xangle = start_angle; xangle < end_angle; xangle += step)
    {
        Ray raycast = this->raycaster.raycast(xangle, this->player.get_position());
        const glm::vec2 relative = glm::vec2(this->player.get_position()*size) - raycast.ray_position*size;
        SDL_RenderDrawLine(renderer, center.x, center.y,
                           relative.x + center.x, relative.y + center.y);
    }
}

