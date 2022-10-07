#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

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

// TODO: Add process handler.
typedef struct player_t
{
	int pid;                // PID of the process.
    uint8_t number;         // Number of the player.
    COORDS position;        // Position of the player.
    uint16_t carried_coins; // Number of coins carried by the player.
    uint16_t brought_coins; // Number of coins brought to the campsite.
    uint16_t deaths;        // Number of deaths of the player.
    DIRECTION direction;    // Direction of the player.
    PLAYER_STATE state;     // State of the player.
    PLAYER_TYPE type;       // Type of the player.
} PLAYER;

void player_init(PLAYER *player, uint8_t number, COORDS position, PLAYER_TYPE type);
void player_move(PLAYER *player, DIRECTION direction, MAP *map);
void player_draw(PLAYER *player);
void player_die(PLAYER *player);

#endif //PLAYER_H
