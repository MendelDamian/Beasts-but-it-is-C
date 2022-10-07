#include <stddef.h>
#include <ncurses.h>

#include "player.h"

void player_init(PLAYER *player, uint8_t number, COORDS position, PLAYER_TYPE type)
{
    if (player == NULL)
    {
        return;
    }

    player->number = number;
    player->position = position;
    player->carried_coins = 0;
    player->brought_coins = 0;
    player->deaths = 0;
    player->direction = NONE;
    player->state = ALIVE;
    player->type = type;
}

void player_move(PLAYER *player, DIRECTION direction, MAP *map)
{
    if (player == NULL)
    {
        return;
    }

    player->direction = direction;

    switch (direction)
    {
        case NORTH:
            if (map->tiles[player->position.y - 1][player->position.x] != TILE_WALL)
            {
                player->position.y--;
            }
            break;

        case SOUTH:
            if (map->tiles[player->position.y + 1][player->position.x] != TILE_WALL)
            {
                player->position.y++;
            }
            break;

        case WEST:
            if (map->tiles[player->position.y][player->position.x - 1] != TILE_WALL)
            {
                player->position.x--;
            }
            break;

        case EAST:
            if (map->tiles[player->position.y][player->position.x + 1] != TILE_WALL)
            {
                player->position.x++;
            }
            break;

        default:
        case NONE:
            break;
    }
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
