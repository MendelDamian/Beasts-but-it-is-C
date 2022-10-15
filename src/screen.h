#ifndef SCREEN_H
#define SCREEN_H

#include "map.h"
#include "server.h"
#include "game.h"

void screen_init(void);
void draw_server_interface(SERVER *server);
void draw_client_interface(MAP_CHUNK *map_chunk, GAME *game, ENTITY *entity);

#endif //SCREEN_H
