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

public:

    glm::ivec2 resolution;
    int pixel_count;
    Sdl_wrapper() = default;

    explicit Sdl_wrapper(const glm::ivec2 &resolution);
    SDL_Window *get_window() const;
    SDL_Renderer *get_renderer() const;
    void lock_framebuffer();
    void unlock_framebuffer();
    void put_framebuffer();
    void clear_framebuffer();
    void set_framebuffer_pixel(const uint32_t &color, const int &index);
    void set_framebuffer_pixel(const uint32_t &color, const glm::ivec2 &position);
    void set_framebuffer_pixel(const glm::ivec3 &color, const int &index);
    void set_framebuffer_pixel(const glm::ivec3 &color, const glm::ivec2 &position);
    void set_color(const glm::ivec3 &color);
    void set_pixel(const glm::ivec2 &position);
    void put_rect(const glm::ivec2 &position, const glm::ivec2 &dimensions);
    void present();
    void clear();
};

#endif //WOLFENSHETIN_SDL_WRAPPER_H
