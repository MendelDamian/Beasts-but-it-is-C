#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

#include "conf.h"
#include "doubly_linked_list.h"
#include "entity.h"
#include "colors.h"
#include "game.h"
#include "treasure.h"

typedef struct server_t
{
    uint8_t number_of_players;
    uint8_t number_of_beasts;
    DLL *entities;
    DLL *dropped_treasures;
    MAP map;
    GAME game;
} SERVER;

void server_main_loop(int server_socket_fd);
void *beast_thread(void *arguments);

#endif //SERVER_H
