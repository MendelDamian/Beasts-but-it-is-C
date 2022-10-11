#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <pthread.h>

#include "direction.h"
#include "coordinate.h"
#include "map.h"

typedef enum player_state_t
{
    ALIVE,
    DEAD
} PLAYER_STATE;

typedef enum player_type_t
{
    HUMAN,
    CPU,
} PLAYER_TYPE;

typedef struct player_t
{
	int pid;                // PID of the process.
    int socket_fd;          // Socket file descriptor.
    pthread_t thread_id;     // Thread ID.
    uint8_t number;         // Number of the player.
    COORDS position;        // Position of the player.
    uint16_t carried_coins; // Number of coins carried by the player.
    uint16_t brought_coins; // Number of coins brought to the campsite.
    uint16_t deaths;        // Number of deaths of the player.
    DIRECTION direction;    // Direction of the player.
    PLAYER_STATE state;     // State of the player.
    PLAYER_TYPE type;       // Type of the player.
} PLAYER;

void player_init(PLAYER *player, COORDS position, PLAYER_TYPE type);
void player_draw(PLAYER *player, uint8_t x_offset, uint8_t y_offset);
void player_die(PLAYER *player);

#endif //PLAYER_H
