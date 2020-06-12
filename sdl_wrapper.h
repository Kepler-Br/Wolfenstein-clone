//
// Created by kepler-br on 6/2/20.
//

#ifndef WOLFENSHETIN_SDL_WRAPPER_H
#define WOLFENSHETIN_SDL_WRAPPER_H

#include <SDL2/SDL.h>
#include <iostream>
#include <glm/glm.hpp>

class Sdl_wrapper
{
private:
    SDL_Texture *framebuffer;
    uint32_t *framebuffer_pixels;
    bool is_framebuffer_locked = false;
    SDL_Window *window = nullptr;
    SDL_Renderer* renderer = nullptr;
    glm::ivec2 resolution_center;
    glm::ivec2 resolution;

    void init_sdl()
    {
        if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
            throw std::runtime_error("Unable to init SDL: " + std::string(SDL_GetError()));

        window = SDL_CreateWindow("Wolfenstein", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, SDL_WINDOW_SHOWN);
        if(window == nullptr)
            throw std::runtime_error("Cannot create SDL2 window: " + std::string(SDL_GetError()));


        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(renderer == nullptr)
            throw std::runtime_error("Cannot create SDL2 renderer: " + std::string(SDL_GetError()));
        this->pixel_count = this->resolution.x * this->resolution.y;
        this->framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, resolution.x, resolution.y);
        if(!this->framebuffer)
            throw std::runtime_error("Cannot create framebuffer texture: " + std::string(SDL_GetError()));
    }

    void calculate_resolution_center()
    {
        resolution_center = glm::ivec2(resolution.x/2, resolution.y/2);
    }

public:

    int pixel_count;
    Sdl_wrapper() = default;

    explicit Sdl_wrapper(const glm::ivec2 &resolution);
    SDL_Window *get_window() const;
    SDL_Renderer *get_renderer() const;
    const glm::ivec2 &get_resolution() const
    {
        return this->resolution;
    }
    const glm::ivec2 &get_resolution_center() const
    {
        return this->resolution_center;
    }
    void lock_framebuffer();
    void unlock_framebuffer();
    void put_framebuffer();
    void clear_framebuffer();
    void set_framebuffer_pixel(const uint32_t &color, const int &index);
    void set_framebuffer_pixel(const uint32_t &color, const glm::ivec2 &position);
    void set_framebuffer_pixel(const glm::ivec3 &color, const int &index);
    void set_framebuffer_pixel(const glm::ivec3 &color, const glm::ivec2 &position);
    void set_color(const glm::ivec3 &color);
    void set_color(const uint8_t &red, const uint8_t &green, const uint8_t &blue);
    void set_pixel(const glm::ivec2 &position);
    void put_rect(const glm::ivec2 &position, const glm::ivec2 &dimensions);
    void present();
    void clear();
};

#endif //WOLFENSHETIN_SDL_WRAPPER_H
