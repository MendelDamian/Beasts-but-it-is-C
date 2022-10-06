#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

#include "player.h"
#include "map.h"

typedef struct server_t
{
    uint8_t number_of_players;
    uint8_t max_players;
    PLAYER *players;
    MAP map;

} SERVER;

#endif //SERVER_H
