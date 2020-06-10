//
// Created by kepler-br on 6/2/20.
//

#ifndef WOLFENSHETIN_MAIN_LOOP_H
#define WOLFENSHETIN_MAIN_LOOP_H

#include "sdl_instance.h"
#include <vector>
#include "input_manager.h"
//#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include "raycaster.h"
#include "world.h"
#include "player.h"
#include "renderer.h"
#include "physics.h"

class Main_loop {
private:
    bool is_running = false;
    bool is_paused = false;
    int render_map_state = 0;
    bool is_player_running = 0;
    float deltatime;
    glm::ivec2 resolution;
    float player_field_of_view = 70.0f*M_PI/180.0f;


    float calc_deltatime();
    void limit_fps();
    void draw_raycast();
    Raycaster raycaster;
    World world;
    Renderer renderer;
    Physics physics;
    Player player;

public:
    Sdl_instance sdl_instance;
    Input_manager input_manager;

    Main_loop(const glm::ivec2 &resolution);

    void engine_update();
    void start();
    void on_update();
    void on_draw();
    void on_event();
    void stop();
};


#endif //WOLFENSHETIN_MAIN_LOOP_H
