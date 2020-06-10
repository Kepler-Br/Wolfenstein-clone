cmake_minimum_required(VERSION 3.16)
project(wolfenshetin)

set(CMAKE_CXX_STANDARD 14)

add_executable(wolfenshetin main.cpp main_loop.cpp main_loop.h sdl_instance.h input_manager.cpp input_manager.h raycaster.cpp raycaster.h world.cpp world.h renderer.cpp renderer.h player.h physics.cpp physics.h state_base.h state_game.cpp state_game.h texture.cpp texture.h)
target_link_libraries(wolfenshetin SDL2 SDL2_image )