#ifndef ENTITY_H
#define ENTITY_H

#include <pthread.h>

#include "direction.h"
#include "coordinate.h"

typedef enum entity_type_t
{
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_BOT,
    ENTITY_TYPE_BEAST,
} ENTITY_TYPE;

typedef struct entity_t
{
    int pid;
    int socket_fd;
    pthread_t thread;
    uint8_t number;

    ENTITY_TYPE type;
    DIRECTION direction;
    COORDS position;
    COORDS spawn_point;

    uint16_t deaths;
    uint16_t carried_coins;
    uint16_t brought_coins;
    uint16_t stagnancy;
} ENTITY;

void entity_init(ENTITY *entity);
const char *get_entity_type_name(ENTITY_TYPE type);

#endif //ENTITY_H
