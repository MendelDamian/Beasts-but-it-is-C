#include <stddef.h>
#include <ncurses.h>

#include "player.h"

void player_init(PLAYER *player, uint8_t number, COORDS position, PLAYER_TYPE type)
{
    if (player == NULL)
    {
        return;
    }

    player->pid = -1;
    player->socket_fd = -1;
    player->number = number;
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

    move(player->position.x, player->position.y);
    mvaddch(player->position.x, player->position.y, player->number + '0');

    attroff(COLOR_PAIR(PAIR_PLAYER));
}
