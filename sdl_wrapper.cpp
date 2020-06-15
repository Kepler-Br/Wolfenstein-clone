#include "sdl_wrapper.h"
#include<stdexcept>
#include <iostream>

void Sdl_wrapper::init_sdl()
{
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
        throw std::runtime_error("Unable to init SDL: " + std::string(SDL_GetError()));

    window = SDL_CreateWindow("Wolfenstein", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, SDL_WINDOW_SHOWN);
    if(window == nullptr)
        throw std::runtime_error("Cannot create SDL2 window: " + std::string(SDL_GetError()));


    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if(renderer == nullptr)
        throw std::runtime_error("Cannot create SDL2 renderer: " + std::string(SDL_GetError()));
    this->pixel_count = this->resolution.x * this->resolution.y;
    this->framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, resolution.x, resolution.y);
    if(!this->framebuffer)
        throw std::runtime_error("Cannot create framebuffer texture: " + std::string(SDL_GetError()));
}

void Sdl_wrapper::calculate_resolution_center()
{
    resolution_center = glm::ivec2(resolution.x/2, resolution.y/2);
}

Sdl_wrapper::Sdl_wrapper(const glm::ivec2 &resolution) :
    resolution(resolution)
{
    this->init_sdl();
    this->calculate_resolution_center();
}

SDL_Window *Sdl_wrapper::get_window() const
{
    return this->window;
}

SDL_Renderer *Sdl_wrapper::get_renderer() const
{
    return this->renderer;
}

const glm::ivec2 &Sdl_wrapper::get_resolution() const
{
    return this->resolution;
}

const glm::ivec2 &Sdl_wrapper::get_resolution_center() const
{
    return this->resolution_center;
}

void Sdl_wrapper::add_y_to_resolution_center(const int &y)
{
    int result = this->resolution_center.y + y;
    if(result >= this->resolution.y || result < 0)
        return;
    this->resolution_center.y += y;
}


void Sdl_wrapper::lock_framebuffer()
{
    static int pitch;
    this->is_framebuffer_locked = true;
    if(SDL_LockTexture(this->framebuffer, nullptr, (void**)&this->framebuffer_pixels, &pitch))
        throw std::runtime_error("Cannot lock framebuffer: " + std::string(SDL_GetError()));
}

void Sdl_wrapper::unlock_framebuffer()
{
    this->is_framebuffer_locked = false;
    SDL_UnlockTexture(this->framebuffer);
}

void Sdl_wrapper::put_framebuffer()
{
    if(this->is_framebuffer_locked)
        throw std::runtime_error("Cannot put framebuffer when it is locked.");
    SDL_RenderCopy(this->renderer, this->framebuffer, nullptr, nullptr);
}

void Sdl_wrapper::clear_framebuffer()
{
    if(!this->is_framebuffer_locked)
        throw std::runtime_error("Cannot clear framebuffer when it is locked.");
    bzero(this->framebuffer_pixels, sizeof(uint32_t)*this->pixel_count);
}


void Sdl_wrapper::set_framebuffer_pixel(const uint32_t &color, const int &index)
{
    if(index >= this->pixel_count)
        throw std::runtime_error("Set pixel index exceeds total number of pixels(uint32_t color, int index).");
    this->framebuffer_pixels[index] = color;
}

void Sdl_wrapper::set_framebuffer_pixel(const Pixel &pixel, const glm::ivec2 &position)
{
    if(position.y >= this->resolution.y || position.x >= this->resolution.x || position.y < 0 || position.x < 0)
        throw std::runtime_error("Set pixel index exceeds total number of pixels(Pixel color, ivec2 position).");
    const int index = position.y * this->resolution.x + position.x;
    ((uint8_t *)(&this->framebuffer_pixels[index]))[0] = pixel.r;
    ((uint8_t *)(&this->framebuffer_pixels[index]))[1] = pixel.g;
    ((uint8_t *)(&this->framebuffer_pixels[index]))[2] = pixel.b;
}

void Sdl_wrapper::set_framebuffer_pixel(const uint32_t &color, const glm::ivec2 &position)
{
    if(position.y >= this->resolution.y || position.x >= this->resolution.x || position.y < 0 || position.x < 0)
        throw std::runtime_error("Set pixel index exceeds total number of pixels(uint32_t color, ivec2 position).");
    const int index = position.y * this->resolution.x + position.x;
    this->framebuffer_pixels[index] = color;

}

void Sdl_wrapper::set_framebuffer_pixel(const glm::ivec3 &color, const int &index)
{
    if(index >= this->pixel_count)
        throw std::runtime_error("Set pixel index exceeds total number of pixels(ivec3 color, int index).");
    ((uint8_t *)(&this->framebuffer_pixels[index]))[0] = color.r;
    ((uint8_t *)(&this->framebuffer_pixels[index]))[1] = color.g;
    ((uint8_t *)(&this->framebuffer_pixels[index]))[2] = color.b;
}

void Sdl_wrapper::set_framebuffer_pixel(const glm::ivec3 &color, const glm::ivec2 &position)
{
    if(position.y >= this->resolution.y || position.x >= this->resolution.x || position.y < 0 || position.x < 0)
        throw std::runtime_error("Set pixel index exceeds total number of pixels(ivec3 color, ivec2 position).");
    const int index = position.y * this->resolution.x + position.x;
    this->set_framebuffer_pixel(color, index);
}

void Sdl_wrapper::set_color(const glm::ivec3 &color)
{
    SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
}

void Sdl_wrapper::set_color(const uint8_t &red, const uint8_t &green, const uint8_t &blue)
{
    SDL_SetRenderDrawColor(this->renderer, red, green, blue, SDL_ALPHA_OPAQUE);
}

void Sdl_wrapper::set_pixel(const glm::ivec2 &position)
{
    SDL_RenderDrawPoint(this->renderer, position.x, position.y);
}

void Sdl_wrapper::put_filled_rect(const glm::ivec2 &position, const glm::ivec2 &dimensions)
{
    SDL_Rect rect = {position.x, position.y, dimensions.x, dimensions.y};
    SDL_RenderFillRect(this->renderer, &rect);
}

void Sdl_wrapper::put_rect(const glm::ivec2 &position, const glm::ivec2 &dimensions)
{
    SDL_Rect rect = {position.x, position.y, dimensions.x, dimensions.y};
    SDL_RenderDrawRect(this->renderer, &rect);
}

void Sdl_wrapper::put_line(const glm::ivec2 &a, const glm::ivec2 &b)
{
    SDL_RenderDrawLine(this->renderer, a.x, a.y, b.x, b.y);
}

void Sdl_wrapper::present()
{
    SDL_RenderPresent(this->renderer);
}

void Sdl_wrapper::clear()
{
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(this->renderer);
}
