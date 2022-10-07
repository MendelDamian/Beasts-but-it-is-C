#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

#include "player.h"
#include "map.h"
#include "colors.h"

#define MAX_PLAYERS 4

typedef struct server_t
{
    int pid;
    uint8_t number_of_players;
    PLAYER players[MAX_PLAYERS];
    MAP map;
    uint32_t turns;

} SERVER;

void server_init(SERVER *server);
void *server_main_loop(void);

#endif //SERVER_H
