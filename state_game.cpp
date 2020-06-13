//
// Created by kepler-br on 6/10/20.
//

#include "state_game.h"

State_game::State_game(Main_loop &main_loop, Input_manager &input_manager, Sdl_wrapper &sdl_instance) :
        State_base(main_loop, main_loop.get_prev_state()),
        sdl_wrapper(sdl_instance),
        input_manager(input_manager),
        player({100, 160}, 360),
        texture_holder(10),
        raycaster(world, lookup),
        renderer(world, sdl_wrapper, player, raycaster, lookup)
{
    this->texture_holder.load("./image_packer/RW24_2.tex", "WALL");
    this->texture_holder.load("./image_packer/DOOR2_4.tex", "DOOR");
    this->texture_holder.load("./image_packer/WALL02_3.tex", "WALL2");
    this->texture_holder.load("./image_packer/WALL69_9.tex", "WALL3");
    this->texture_holder.load("./image_packer/WALL03_7.tex", "WALL4");
    this->lookup.init(this->world.get_block_size(), this->sdl_wrapper.get_resolution().x);
//    SDL_ShowCursor(SDL_DISABLE);
//    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void State_game::on_draw()
{
    auto *renderer = this->sdl_wrapper.get_renderer();

    this->sdl_wrapper.set_color(255, 255, 255);
    this->renderer.draw_map();
    this->renderer.draw_world();
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

}

void State_game::engine_update()
{

}

void State_game::on_event()
{
    const float deltatime = this->main_loop.get_deltatime();
    const glm::ivec2 mouse_delta = this->input_manager.getDeltaMouseCoord();
//    this->player.add_x_view_angle(mouse_delta.x/500.0f);
//    this->player.add_y_view_angle(-mouse_delta.y*1.8f);
    if(this->input_manager.isKeyDown(SDLK_w))
    {
        const glm::vec2 forward = {this->lookup.cos(this->player.get_x_view_angle()),
                                   this->lookup.sin(this->player.get_x_view_angle())};
        const glm::vec2 velocity = {forward.x * this->player.get_speed() * deltatime,
                                    forward.y * this->player.get_speed() * deltatime};
        this->player.add_position(velocity);
    }
    if(this->input_manager.isKeyDown(SDLK_s))
    {
        const glm::vec2 forward = {this->lookup.cos(this->player.get_x_view_angle()),
                                   this->lookup.sin(this->player.get_x_view_angle())};
        const glm::vec2 velocity = {forward.x * this->player.get_speed() * deltatime,
                                    forward.y * this->player.get_speed() * deltatime};
        this->player.add_position(-velocity);
    }
    if(this->input_manager.isKeyDown(SDLK_a))
    {
//        const glm::vec2 right = glm::vec2(-this->player.get_forward().y, this->player.get_forward().x);
//        glm::ivec2 velocity = float(this->player.get_speed()) * right * deltatime;
//        this->player.add_position(velocity);
//        const glm::vec2 right = glm::vec2(-this->player.get_forward().y, this->player.get_forward().x);
//        glm::vec2 velocity = -100.0f * right * deltatime;
//        if(this->is_player_running)
//            velocity *= 2.0f;
//        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
//        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_d))
    {
//        const glm::vec2 left = glm::vec2(this->player.get_forward().y, -this->player.get_forward().x);
//        glm::ivec2 velocity = float(this->player.get_speed()) * left * deltatime;
//        this->player.add_position(velocity);
//        const glm::vec2 right = glm::vec2(this->player.get_forward().y, -this->player.get_forward().x);
//        glm::vec2 velocity = -100.0f * right * deltatime;
//        if(this->is_player_running)
//            velocity *= 2.0f;
//        glm::vec3 new_position = this->physics.move(this->player.get_position(), glm::vec3(velocity, 0.0f));
//        this->player.set_position(new_position);
    }
    if(this->input_manager.isKeyDown(SDLK_RIGHT))
    {
        this->player.add_x_view_angle(52);
        const int angle = this->player.get_x_view_angle();
        if (angle>this->lookup.angle360)
            this->player.add_x_view_angle(-this->lookup.angle360);
    }
    if(this->input_manager.isKeyDown(SDLK_LEFT))
    {
        this->player.add_x_view_angle(-52);
        const int angle = this->player.get_x_view_angle();
        if (angle<0)
            this->player.add_x_view_angle(this->lookup.angle360);
    }
//    if(this->input_manager.isKeyDown(SDLK_UP))
//        this->player.add_y_view_angle(640 * deltatime);
//    if(this->input_manager.isKeyDown(SDLK_DOWN))
//        this->player.add_y_view_angle(-640 * deltatime);
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
//    std::cout << 1.0f/this->main_loop.get_deltatime() << std::endl;

}

void State_game::on_predraw()
{
    this->sdl_wrapper.clear();
}

void State_game::on_postdraw()
{
    this->sdl_wrapper.present();
}

void State_game::preload()
{

}
