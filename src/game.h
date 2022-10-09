#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#include "conf.h"
#include "map.h"

typedef struct game_t
{
    uint16_t server_pid;
    MAP map;
    uint32_t turns;
    int *server_socket_fd;
} GAME;

#endif //GAME_H
