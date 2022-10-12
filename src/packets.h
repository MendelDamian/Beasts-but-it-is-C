#ifndef PACKETS_H
#define PACKETS_H

#include "direction.h"
#include "map.h"
#include "player.h"

typedef struct packet_data_t
{
    int pid;  // PID of the client's process.
    DIRECTION direction;  // Direction of the move.
    bool quit;  // Whether the client wants to quit.
} PACKET_DATA;

typedef struct packet_data_response_t
{
    int pid;  // PID of the server's process.
    uint8_t number;  // Number of the player.
    MAP_CHUNK chunk;  // Chunk of the map.
    uint32_t turns;  // Number of turns.
    uint16_t carried_coins;  // Number of coins carried by the player.
    uint16_t brought_coins;  // Number of coins brought to the campsite.
    uint16_t deaths;  // Number of deaths of the player.
    COORDS position;  // Position of the player.
    PLAYER_STATE state;  // State of the player.
    bool end;  // Whether the game has ended.
} PACKET_DATA_RESPONSE;

#endif //PACKETS_H
