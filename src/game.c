#include "game.h"

void game_init(GAME *game)
{
    if (game == NULL)
    {
        return;
    }

    game->server_socket_fd = -1;
    game->server_pid = -1;
    game->turns = 0;
}
