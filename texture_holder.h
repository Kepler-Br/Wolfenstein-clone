//
// Created by kepler-br on 6/11/20.
//

#ifndef WOLFENSTEIN_TEXTURE_HOLDER_H
#define WOLFENSTEIN_TEXTURE_HOLDER_H

#include "texture.h"
#include <vector>
#include <map>
#include <stdexcept>

class Texture_holder
{
private:
    std::vector<Texture*> textures;
    std::map<std::string, Texture*> texture_names;

public:
    Texture_holder(int reserve_texture_count)
    {
        this->textures.reserve(reserve_texture_count);
    }

    Texture_holder()
    {
        this->textures.reserve(10);
    }

    ~Texture_holder()
    {
        for(auto *texture : this->textures)
        {
            texture->free();
            delete texture;
        }
    }

    const int get_total() const
    {
        return this->textures.size();
    }

    const Texture &get_by_id(const int &id) const
    {
        if (id >= this->textures.size())
            throw std::runtime_error("Tried to access non existing texture by id.");
        return *this->textures[id];
    }

    const Texture &get_by_name(const std::string &name) const
    {
        if(this->texture_names.find(name) == this->texture_names.end())
            throw std::runtime_error("Tried to access non existing texture by name.");
        return *this->texture_names.at(name);
    }

    int load(const std::string &path, const std::string &name)
    {
        auto *texture = new Texture;
        texture->read(path);
        this->textures.push_back(texture);
        if(this->texture_names.find(name) != this->texture_names.end())
            throw std::runtime_error("Tried to add existing texture to texture names.");
        this->texture_names[name] = texture;
    }
};


#endif //WOLFENSTEIN_TEXTURE_HOLDER_H
