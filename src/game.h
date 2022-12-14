#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#include "map.h"

typedef struct game_t
{
    int server_socket_fd;
    int server_pid;
    COORDS campsite;
    volatile bool running;
    uint32_t turns;
} GAME;

void game_init(GAME *game);

#endif //GAME_H
