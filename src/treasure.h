#ifndef TREASURE_H
#define TREASURE_H

#include "coordinate.h"

typedef struct treasure_t
{
    COORDS position;
    uint16_t coins;
} TREASURE;

#endif //TREASURE_H
