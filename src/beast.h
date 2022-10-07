#ifndef BEAST_H
#define BEAST_H

#include <stdint.h>

#include "direction.h"
#include "coordinate.h"

typedef struct beast_t
{
    COORDS position;
    DIRECTION direction;
    // Thread handle
} BEAST;

#endif //BEAST_H
