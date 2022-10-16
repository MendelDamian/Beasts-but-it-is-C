#ifndef SCREEN_H
#define SCREEN_H

#include "map.h"
#include "server.h"
#include "game.h"

void screen_init(void);
void draw_server_interface(SERVER *server, bool full_draw);
void draw_client_interface(MAP_CHUNK *map_chunk, GAME *game, ENTITY *entity, bool full_draw);

#endif //SCREEN_H
