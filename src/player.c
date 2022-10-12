#include <stddef.h>
#include <ncurses.h>

#include "player.h"

void player_init(PLAYER *player, COORDS position, PLAYER_TYPE type)
{
    if (player == NULL)
    {
        return;
    }

    player->pid = -1;
    player->socket_fd = -1;
    player->thread_id = 0;
    player->position = position;
    player->carried_coins = 0;
    player->brought_coins = 0;
    player->deaths = 0;
    player->direction = NONE;
    player->state = ALIVE;
    player->type = type;
}

void player_draw(PLAYER *player)
{
    if (player == NULL)
    {
        return;
    }

    attron(COLOR_PAIR(PAIR_PLAYER));

    mvaddch(player->position.y + INTERFACE_OFFSET_Y, player->position.x + INTERFACE_OFFSET_X, player->number + '0');

    attroff(COLOR_PAIR(PAIR_PLAYER));
}
