#ifndef SCREEN_H
#define SCREEN_H

#include <stdlib.h>

#include "map.h"
#include "server.h"
#include "game.h"

typedef struct interface_t
{
    WINDOW *win;
    uint8_t width;
    uint8_t height;
} INTERFACE;

INTERFACE *interface_init(void);
void interface_destroy(INTERFACE *interface);
void draw_server_interface(INTERFACE *interface, SERVER *server);
void draw_client_interface(INTERFACE *interface, MAP_CHUNK *map_chunk, GAME *game, ENTITY *entity);
void draw_entity(INTERFACE *interface, ENTITY *entity);
void draw_treasures(INTERFACE *interface, SERVER *server);
void draw_map(INTERFACE *interface, MAP *map);
void draw_map_chunk(INTERFACE *interface, MAP_CHUNK *chunk);

#endif //SCREEN_H
