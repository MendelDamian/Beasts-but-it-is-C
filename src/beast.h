#ifndef BEAST_H
#define BEAST_H

#include <stdint.h>

#include "direction.h"

typedef struct beast_t
{
    uint8_t x;
    uint8_t y;
    DIRECTION direction;
    // Thread handle
} BEAST;

#endif //BEAST_H
