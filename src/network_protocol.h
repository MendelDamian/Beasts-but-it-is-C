#ifndef NETWORK_PROTOCOL_H
#define NETWORK_PROTOCOL_H

#include <sys/socket.h>

#include "direction.h"
#include "entity.h"
#include "game.h"

typedef enum packet_type_t
{
    PACKET_TYPE_CLIENT_HANDSHAKE,
    PACKET_TYPE_CLIENT_MOVE,
    PACKET_TYPE_CLIENT_QUIT,
    PACKET_TYPE_SERVER_HANDSHAKE,
    PACKET_TYPE_SERVER_FULL,
    PACKET_TYPE_SERVER_GAME_DATA,
    PACKET_TYPE_SERVER_ADD_BEAST,
    PACKET_TYPE_SERVER_GAME_END,
} PACKET_TYPE;

struct client_handshake_t
{
    int pid;
    ENTITY_TYPE type;
};

struct client_move_t
{
    DIRECTION direction;
};

struct server_handshake_t
{
    int pid;
    COORDS spawn_point;
    uint8_t number;
    uint32_t turns;
};

struct server_game_data_t
{
    COORDS position;
    MAP_CHUNK chunk;
    uint16_t deaths;
    uint16_t carried_coins;
    uint16_t brought_coins;
};

typedef struct packet_t
{
    PACKET_TYPE type;
    union
    {
        struct client_handshake_t client_handshake;
        struct client_move_t client_move;
        struct server_handshake_t server_handshake;
        struct server_game_data_t server_game_data;
    };
} PACKET;

ssize_t send_client_handshake(int socket_fd, ENTITY *entity);
ssize_t send_client_move(int socket_fd, ENTITY *entity);
ssize_t send_client_quit(int socket_fd);

ssize_t send_server_handshake(int socket_fd, ENTITY *entity, GAME *game);
ssize_t send_server_full(int socket_fd);
ssize_t send_server_game_data(int socket_fd, ENTITY *entity, MAP_CHUNK *chunk);
ssize_t send_server_add_beast(int socket_fd);
ssize_t send_server_game_end(int socket_fd);

ssize_t recv_packet(int socket_fd, PACKET *packet);

#endif //NETWORK_PROTOCOL_H
