//
// Created by kepler-br on 6/2/20.
//

#ifndef WOLFENSHETIN_SDL_WRAPPER_H
#define WOLFENSHETIN_SDL_WRAPPER_H

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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

    void init_sdl();
    void calculate_resolution_center();

public:

    int pixel_count;
    Sdl_wrapper() = default;

    explicit Sdl_wrapper(const glm::ivec2 &resolution);
    SDL_Window *get_window() const;
    SDL_Renderer *get_renderer() const;
    const glm::ivec2 &get_resolution() const;

    const glm::ivec2 &get_resolution_center() const;

    // Framebuffer operations.
    void lock_framebuffer();
    void unlock_framebuffer();
    void put_framebuffer();
    void clear_framebuffer();
    void set_framebuffer_pixel(const uint32_t &color, const int &index);
    void set_framebuffer_pixel(const uint32_t &color, const glm::ivec2 &position);
    void set_framebuffer_pixel(const glm::ivec3 &color, const int &index);
    void set_framebuffer_pixel(const glm::ivec3 &color, const glm::ivec2 &position);

    // Direct SDL2 operations.
    void set_color(const glm::ivec3 &color);
    void set_color(const uint8_t &red, const uint8_t &green, const uint8_t &blue);
    void set_pixel(const glm::ivec2 &position);
    void put_filled_rect(const glm::ivec2 &position, const glm::ivec2 &dimensions);
    void put_rect(const glm::ivec2 &position, const glm::ivec2 &dimensions);
    void put_line(const glm::ivec2 &a, const glm::ivec2 &b)
    {
        SDL_RenderDrawLine(this->renderer, a.x, a.y, b.x, b.y);
    }
    void present();
    void clear();
};


#endif //WOLFENSHETIN_SDL_WRAPPER_H
