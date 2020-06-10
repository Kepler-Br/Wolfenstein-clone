//
// Created by kepler-br on 6/2/20.
//

#ifndef WOLFENSHETIN_SDL_INSTANCE_H
#define WOLFENSHETIN_SDL_INSTANCE_H

#include <SDL2/SDL.h>
#include <iostream>
#include <glm/glm.hpp>

class Sdl_instance {
public:
    SDL_Window *window = nullptr;
    SDL_Renderer* renderer = nullptr;
    glm::ivec2 resolution;
    Sdl_instance() = default;

    explicit Sdl_instance(const glm::ivec2 &resolution):
        resolution(resolution)
    {
        if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
        {
            std::cerr << "Unable to init SDL, error: " << SDL_GetError() << std::endl;
            exit(-1);
        }

        window = SDL_CreateWindow("Wolfenstein", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, SDL_WINDOW_SHOWN);

        if(window == nullptr)
        {
            std::cerr << "Cannot create window: " << SDL_GetError() << std::endl;
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if(renderer == nullptr)
        {
            std::cerr << "Cannot create renderer: " << SDL_GetError() << std::endl;
            exit(1);
        }
    }
};

#endif //WOLFENSHETIN_SDL_INSTANCE_H
