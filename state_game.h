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
    State_game(Main_loop &main_loop, Input_manager &input_manager, Sdl_instance &sdl_instance);
    void engine_update() override;
    void on_event() override;
    void on_update() override;
    void on_predraw() override;
    void on_draw() override;
    void on_postdraw() override;
};


#endif //WOLFENSHETIN_STATE_GAME_H
