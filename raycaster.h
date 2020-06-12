#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "world.h"
#include "lookup_table.h"

class Raycaster
{
    const World &world;
    Lookup_table lookup;
public:
    Raycaster(const World& world);
    Ray cast(glm::ivec2 position, const int degree);
};

#endif // RAYCASTER_H
