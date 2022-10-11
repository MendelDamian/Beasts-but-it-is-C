#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

#include "conf.h"
#include "player.h"
#include "map.h"
#include "colors.h"

typedef struct server_t
{
    uint8_t number_of_players;
    PLAYER players[MAX_PLAYERS];
} SERVER;

void server_init(SERVER *server);
void server_main_loop(int server_socket_fd);

void player_move(PLAYER *player, DIRECTION direction, MAP *map);

#endif //SERVER_H
