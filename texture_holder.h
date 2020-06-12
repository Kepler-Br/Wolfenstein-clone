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
    std::map<std::string, std::pair<Texture*, size_t>> texture_names;

public:
    Texture_holder(const int &reserve_texture_count = 10);

    Texture_holder();
    ~Texture_holder();
    int get_total() const;
    const Texture &get_by_id(const size_t &id) const;
    const Texture &get_by_name(const std::string &name) const;
    int load(const std::string &path, const std::string &name);
    void unload_all();
    void unload(const size_t &id);
    void unload(const std::string &name);
};


#endif //WOLFENSTEIN_TEXTURE_HOLDER_H
