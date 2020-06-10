//
// Created by kepler-br on 6/2/20.
//

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "main_loop.h"
#include <cmath>

Main_loop::Main_loop(const glm::ivec2 &resolution):
    raycaster(world),
    player({68.5f, 68.5f, 0.0f}, 0.0f),
    renderer(world, raycaster, player, sdl_instance),
    physics(world, raycaster, player)
{
    this->sdl_instance = Sdl_instance(resolution);
    this->resolution = resolution;
}

void Main_loop::engine_update()
{

}
void Main_loop::start()
{
    this->is_running = true;
    while(this->is_running)
    {
        this->input_manager.update();
        this->on_event();
        this->engine_update();
        if (!this->is_paused)
            this->on_update();
        const auto &renderer = this->sdl_instance.renderer;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        this->on_draw();
        SDL_RenderPresent(renderer);
        this->deltatime = this->calc_deltatime();
        this->limit_fps();
    }
}
void Main_loop::on_update()
{

}

void Main_loop::draw_raycast()
{

}

void Main_loop::on_draw()
{
    const auto &renderer = this->sdl_instance.renderer;

//    for(int x = 0; x < this->world.get_world_dimensions().x; x++)
//    {
//        for(int y = 0; y < this->world.get_world_dimensions().y; y++)
//        {
//            SDL_Rect rect = {0+(int)(x*this->world.get_block_size()),0+int(y*this->world.get_block_size()), (int)this->world.get_block_size(), (int)this->world.get_block_size()};
//            if (this->world.get_block(y*this->world.get_world_dimensions().x + x) != 0)
//            {
//                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
//                SDL_RenderFillRect(renderer, &rect);
//                SDL_SetRenderDrawColor(renderer, 124, 124, 124, SDL_ALPHA_OPAQUE);
//                SDL_RenderDrawRect(renderer, &rect);
//            }
//            else
//            {
//                SDL_SetRenderDrawColor(renderer, 124, 124, 124, SDL_ALPHA_OPAQUE);
//                SDL_RenderDrawRect(renderer, &rect);
//            }
//        }
//    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    this->renderer.render_world();

    if(this->render_map_state > 0)
        if(this->render_map_state == 1)
            this->renderer.render_map(false, false, 0.5f);
        else
            this->renderer.render_map(false, true, 0.5f);
//    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
//    draw_raycast();
//    SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
//    SDL_RenderDrawLine(renderer, this->player_position.x, this->player_position.y,
//        this->player_position.x + this->player_forward.x*10, this->player_position.y + this->player_forward.y*10);
}
void Main_loop::on_event()
{
    if(this->input_manager.isKeyDown(SDLK_q))
    {
        const float &block_size = this->world.get_block_size();
        this->world.set_block_size(block_size-1.0f);
        std::cout << block_size << std::endl;
    }
    if(this->input_manager.isKeyDown(SDLK_e))
    {
        const float &block_size = this->world.get_block_size();
        this->world.set_block_size(block_size+1.0f);
        std::cout << block_size << std::endl;
    }
    if(this->input_manager.isKeyDown(SDLK_w))
    {
        glm::vec2 velocity = 100.0f * this->player.get_forward() * this->deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_s))
    {
        glm::vec2 velocity = -100.0f * this->player.get_forward() * this->deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_a))
    {
        const glm::vec2 right = glm::vec2(-this->player.get_forward().y, this->player.get_forward().x);
        glm::vec2 velocity = -100.0f * right * this->deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_d))
    {
        const glm::vec2 right = glm::vec2(this->player.get_forward().y, -this->player.get_forward().x);
        glm::vec2 velocity = -100.0f * right * this->deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_LEFT))
        this->player.add_view_angle(-M_PI / 1.0f * this->deltatime);
    if(this->input_manager.isKeyDown(SDLK_RIGHT))
        this->player.add_view_angle(M_PI / 1.0f * this->deltatime);
    if(this->input_manager.isKeyDown(SDLK_ESCAPE))
        this->stop();
    if(this->input_manager.isEventPending(SDL_QUIT))
        this->stop();
    if(this->input_manager.isKeyPressed(SDLK_TAB))
    {
        this->render_map_state++;
        if(this->render_map_state > 2)
            this->render_map_state = 0;
    }
    if(this->input_manager.isKeyPressed(SDLK_LSHIFT))
        this->is_player_running = true;
    if(this->input_manager.isKeyReleased(SDLK_LSHIFT))
        this->is_player_running = false;
}

void Main_loop::stop()
{
    this->is_running = false;
}

float Main_loop::calc_deltatime()
{
    static float lastTime = 0;
    static float currentTime = 0;

    if (currentTime > lastTime)
        lastTime = currentTime;
    currentTime = SDL_GetTicks();
    return ((currentTime - lastTime) / 1000.0f);
}

void Main_loop::limit_fps()
{
    static const uint max_fps = 30;
    static uint start = SDL_GetTicks();
    if (((1000 / max_fps) > (SDL_GetTicks() - start)))
        SDL_Delay(1000 / max_fps - (SDL_GetTicks() - start));
    start = SDL_GetTicks();
}