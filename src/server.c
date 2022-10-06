#include "server.h"
#include <stdlib.h>

void *main_loop(void)
{
    MAP map;
    map.height = 20;
    map.width = 60;

    SERVER server;
    server.max_players = 4;
    server.number_of_players = 0;

    return NULL;
}
