#include "network_protocol.h"

ssize_t send_client_handshake(int socket_fd, ENTITY *entity)
{
    PACKET packet;
    packet.type = PACKET_TYPE_CLIENT_HANDSHAKE;
    packet.client_handshake.pid = entity->pid;
    packet.client_handshake.type = entity->type;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t send_client_move(int socket_fd, ENTITY *entity)
{
    PACKET packet;
    packet.type = PACKET_TYPE_CLIENT_MOVE;
    packet.client_move.direction = entity->direction;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t send_client_quit(int socket_fd)
{
    PACKET packet;
    packet.type = PACKET_TYPE_CLIENT_QUIT;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t send_server_handshake(int socket_fd, ENTITY *entity, GAME *game)
{
    PACKET packet;
    packet.type = PACKET_TYPE_SERVER_HANDSHAKE;
    packet.server_handshake.pid = entity->pid;
    packet.server_handshake.spawn_point = entity->spawn_point;
    packet.server_handshake.number = entity->number;
    packet.server_handshake.turns = game->turns;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t send_server_full(int socket_fd)
{
    PACKET packet;
    packet.type = PACKET_TYPE_SERVER_FULL;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t send_server_game_data(int socket_fd, ENTITY *entity, MAP_CHUNK *chunk)
{
    PACKET packet;
    packet.type = PACKET_TYPE_SERVER_GAME_DATA;
    packet.server_game_data.position = entity->position;
    packet.server_game_data.chunk = *chunk;
    packet.server_game_data.deaths = entity->deaths;
    packet.server_game_data.carried_coins = entity->carried_coins;
    packet.server_game_data.brought_coins = entity->brought_coins;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t send_server_add_beast(int socket_fd)
{
    PACKET packet;
    packet.type = PACKET_TYPE_SERVER_ADD_BEAST;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t send_server_game_end(int socket_fd)
{
    PACKET packet;
    packet.type = PACKET_TYPE_SERVER_GAME_END;

    return send(socket_fd, &packet, sizeof(PACKET), 0);
}

ssize_t recv_packet(int socket_fd, PACKET *packet)
{
    return recv(socket_fd, packet, sizeof(PACKET), 0);
}
