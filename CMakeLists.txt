cmake_minimum_required(VERSION 3.16)
project(wolfenstein)

set(CMAKE_CXX_STANDARD 14)

add_executable(wolfenstein main.cpp main_loop.cpp main_loop.h sdl_wrapper.h input_manager.cpp input_manager.h raycaster.cpp raycaster.h world.cpp world.h world_renderer.cpp world_renderer.h player.h physics.cpp physics.h state_base.h state_game.cpp state_game.h texture.cpp texture.h sdl_wrapper.cpp world_renderer_thread.h types.h texture_holder.cpp texture_holder.h)
target_link_libraries(wolfenstein SDL2 pthread)