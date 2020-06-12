#include "sdl_wrapper.h"

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
//    glm::ivec2 new_position = position;
//    if(position.x < 0)
//        new_position.x = 0;
//    if(position.y < 0)
//        new_position.y = 0;
//    if(position.x >= this->sdl_wrapper.resolution.x)
//        new_position.x = this->sdl_wrapper.resolution.x-1;
//    if(position.y >= this->sdl_wrapper.resolution.y)
//        new_position.y = this->sdl_wrapper.resolution.y-1;
//    int index = new_position.y * this->sdl_wrapper.resolution.x + new_position.x;
    this->framebuffer_pixels[index] = color;
}

void Sdl_wrapper::set_framebuffer_pixel(const uint32_t &color, const glm::ivec2 &position)
{
    int index = position.y * this->resolution.x + position.x;
    if(index >= this->pixel_count)
        throw std::runtime_error("Set pixel index exceeds total number of pixels(uint32_t color, ivec2 position).");
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
    int index = position.y * this->resolution.x + position.x;
    if(index >= this->pixel_count)
        throw std::runtime_error("Set pixel index exceeds total number of pixels(ivec3 color, ivec2 position).");
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

void Sdl_wrapper::put_rect(const glm::ivec2 &position, const glm::ivec2 &dimensions)
{

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
