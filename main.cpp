#include <iostream>
#include "main_loop.h"
#include "state_game.h"
#include "texture.h"

int main()
{
    Main_loop main_loop({800, 600});
    auto *game = new State_game(main_loop, main_loop.get_input_manager(), main_loop.get_sdl_instance());
    main_loop.push_state((State_base *)game);
    main_loop.start();
    return 0;
}
