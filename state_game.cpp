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
}