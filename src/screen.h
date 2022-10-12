#ifndef SCREEN_H
#define SCREEN_H

#include "map.h"
#include "server.h"
#include "game.h"

void screen_init(void);
void draw_server_interface(SERVER *server, GAME *game);
void draw_client_interface(MAP_CHUNK *map_chunk, GAME *game, PLAYER *player);

#endif //SCREEN_H
