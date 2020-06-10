//
// Created by kepler-br on 6/10/20.
//

#ifndef WOLFENSHETIN_STATE_GAME_H
#define WOLFENSHETIN_STATE_GAME_H


#include "main_loop.h"
#include "state_base.h"
#include "sdl_instance.h"
#include "raycaster.h"
#include "world.h"
#include "player.h"
#include "renderer.h"
#include "physics.h"
#include "input_manager.h"

class State_game: State_base
{
private:
    int render_map_state = 0;
    bool is_player_running = false;
    bool is_paused = false;
    Sdl_instance &sdl_instance;
    Input_manager &input_manager;
    Raycaster raycaster;
    World world;
    Renderer renderer;
    Physics physics;
    Player player;
    State_base *prev_state;

public:
    State_game(Main_loop &main_loop, Input_manager &input_manager, Sdl_instance &sdl_instance):
    State_base(main_loop, main_loop.get_prev_state()),
    input_manager(input_manager),
    raycaster(world),
    player({68.5f, 68.5f, 0.0f}, 0.0f),
    renderer(world, raycaster, player, sdl_instance),
    physics(world, raycaster, player),
    sdl_instance(sdl_instance)
    {
        this->prev_state = prev_state;
    }
    void engine_update() override
    {

    };
    void on_event() override
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
    };
    void on_update() override
    {

    };
    void on_predraw() override
    {
        const auto &renderer = this->sdl_instance.renderer;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
    };
    void on_draw() override;
    void on_postdraw() override
    {
        const auto &renderer = this->sdl_instance.renderer;
        SDL_RenderPresent(renderer);
    };
    void on_stop() override
    {

    };
};


#endif //WOLFENSHETIN_STATE_GAME_H
