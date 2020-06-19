#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "world.h"
#include "lookup_table.h"

class Raycaster
{
    const World &world;
    Lookup_table &lookup;
public:
    Raycaster(const World& world, Lookup_table &lookup_table);
    Ray cast(glm::vec2 position, int degree);
    Ray cast(glm::vec2 position, int degree, int depth);
    Ray cast_one(const glm::vec2 &start_position, int degree);
};

#endif // RAYCASTER_H
