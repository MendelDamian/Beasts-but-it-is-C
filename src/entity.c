#include <string.h>
#include <ncurses.h>

#include "entity.h"
#include "colors.h"
#include "map.h"

void entity_init(ENTITY *entity)
{
    if (entity == NULL)
    {
        return;
    }

    memset(entity, 0, sizeof(ENTITY));
}

void draw_entity(ENTITY *entity)
{
    if (entity == NULL)
    {
        return;
    }

    switch (entity->type)
    {
        case ENTITY_TYPE_BOT:
        case ENTITY_TYPE_PLAYER:
            attron(COLOR_PAIR(PAIR_PLAYER));
            mvaddch(entity->position.y + INTERFACE_OFFSET_Y, entity->position.x + INTERFACE_OFFSET_X, entity->number + '0');
            attroff(COLOR_PAIR(PAIR_PLAYER));
            break;

        case ENTITY_TYPE_BEAST:
            attron(COLOR_PAIR(PAIR_BEAST));
            mvaddch(entity->position.y + INTERFACE_OFFSET_Y, entity->position.x + INTERFACE_OFFSET_X, TILE_BEAST);
            attroff(COLOR_PAIR(PAIR_BEAST));
            break;

        default:
            break;
    }
}

const char *get_entity_type_name(ENTITY_TYPE type)
{
    switch (type)
    {
        case ENTITY_TYPE_PLAYER:
            return "HUMAN";
        case ENTITY_TYPE_BOT:
            return "CPU";
        case ENTITY_TYPE_BEAST:
            return "BEAST";
        default:
            return "UNKNOWN";
    }
}
