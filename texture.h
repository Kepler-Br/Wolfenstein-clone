//
// Created by kepler-br on 6/10/20.
//

#ifndef WOLFENSHETIN_TEXTURE_H
#define WOLFENSHETIN_TEXTURE_H

#include <string>
#include <glm/glm.hpp>

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class Texture
{
private:
    bool transparent;
    bool wrap = false;
    Pixel *pixels = nullptr;
    glm::ivec2 resolution;
    uint pixel_count;

    void read_file(const std::string &path);

public:
    Texture(const std::string &path);
    Texture() = default;

    Pixel *get_pixels() const;
    const Pixel &get_pixel(const uint &index) const;
    const Pixel &get_pixel(const glm::ivec2 &position) const;
    const uint &get_pixel_count() const;
    const glm::ivec2 &get_resolution() const;
    const bool is_transparent() const;
    void set_wrapping(const bool &wrap);
    void read(const std::string &path);
};


#endif //WOLFENSHETIN_TEXTURE_H
