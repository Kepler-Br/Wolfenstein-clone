//
// Created by kepler-br on 6/10/20.
//

#include "state_game.h"

void State_game::on_draw()
{
    const auto &renderer = this->sdl_instance.renderer;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    this->renderer.render_world();

    if(this->render_map_state == 1)
        this->renderer.render_map(false, false, 0.5f);
    else if(this->render_map_state == 2)
        this->renderer.render_map(false, true, 0.5f);
//    for(int x = 0; x < this->texture.get_resolution().x; x++)
//    {
//        for(int y = 0; y < this->texture.get_resolution().y; y++)
//        {
//            const Pixel &pixel = texture.get_pixel(glm::ivec2(x, y));
//            SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, SDL_ALPHA_OPAQUE);
//            SDL_RenderDrawPoint(renderer, x, y);
//        }
//    }
}

State_game::State_game(Main_loop &main_loop, Input_manager &input_manager, Sdl_instance &sdl_instance) :
        State_base(main_loop, main_loop.get_prev_state()),
        input_manager(input_manager),
        raycaster(world),
        player({68.5f, 68.5f, 0.0f}, 0.0f),
        renderer(world, raycaster, player, sdl_instance),
        physics(world, raycaster, player),
        sdl_instance(sdl_instance)
{
}

void State_game::engine_update()
{

}

void State_game::on_event()
{
    const float deltatime = this->main_loop.get_deltatime();
    if(this->input_manager.isKeyDown(SDLK_w))
    {
        glm::vec2 velocity = 100.0f * this->player.get_forward() * deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_s))
    {
        glm::vec2 velocity = -100.0f * this->player.get_forward() * deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_a))
    {
        const glm::vec2 right = glm::vec2(-this->player.get_forward().y, this->player.get_forward().x);
        glm::vec2 velocity = -100.0f * right * deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_d))
    {
        const glm::vec2 right = glm::vec2(this->player.get_forward().y, -this->player.get_forward().x);
        glm::vec2 velocity = -100.0f * right * deltatime;
        if(this->is_player_running)
            velocity *= 2.0f;
        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_LEFT))
        this->player.add_view_angle(-M_PI / 1.0f * deltatime);
    if(this->input_manager.isKeyDown(SDLK_RIGHT))
        this->player.add_view_angle(M_PI / 1.0f * deltatime);
    if(this->input_manager.isKeyDown(SDLK_ESCAPE))
        this->main_loop.stop();
    if(this->input_manager.isEventPending(SDL_QUIT))
        this->main_loop.stop();
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

void State_game::on_update()
{

}

void State_game::on_predraw()
{
    const auto &renderer = this->sdl_instance.renderer;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void State_game::on_postdraw()
{
    const auto &renderer = this->sdl_instance.renderer;
    SDL_RenderPresent(renderer);
}
