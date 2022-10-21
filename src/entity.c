#include <string.h>

#include "entity.h"

void entity_init(ENTITY *entity)
{
    if (entity == NULL)
    {
        return;
    }

    memset(entity, 0, sizeof(ENTITY));
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
