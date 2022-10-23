#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#include "network_protocol.h"
#include "client.h"
#include "game.h"
#include "timer.h"
#include "interface.h"

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct client_listener_args_t
{
    GAME *game;
    ENTITY *entity;
    MAP_CHUNK *chunk;
    INTERFACE *interface;
} CLIENT_LISTENER_ARGS;

static void on_key_pressed(int key, GAME *game, ENTITY *entity)
{
    if (game == NULL || entity == NULL)
    {
        return;
    }

    switch (key)
    {
        case 'Q':
        case 'q':
            send_client_quit(game->server_socket_fd);
            close(game->server_socket_fd);
            game->running = false;
            break;

        case 'W':
        case 'w':
        case KEY_UP:
            entity->direction = NORTH;
            break;

        case 'A':
        case 'a':
        case KEY_LEFT:
            entity->direction = WEST;
            break;

        case 'S':
        case 's':
        case KEY_DOWN:
            entity->direction = SOUTH;
            break;

        case 'D':
        case 'd':
        case KEY_RIGHT:
            entity->direction = EAST;
            break;

        default:
            break;
    }
}

static void *listener_thread(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    CLIENT_LISTENER_ARGS args = *(CLIENT_LISTENER_ARGS *)arguments;
    if (args.game == NULL || args.entity == NULL || args.chunk == NULL || args.interface == NULL)
    {
        return NULL;
    }

    GAME *game = args.game;
    ENTITY *entity = args.entity;
    MAP_CHUNK *chunk = args.chunk;
    INTERFACE *interface = args.interface;

    while (game->running)
    {
        PACKET packet;
        ssize_t bytes_received = recv_packet(game->server_socket_fd, &packet);
        if (bytes_received <= 0)
        {
            // Server closed connection or something went wrong.
            game->running = false;
            close(game->server_socket_fd);
            return NULL;
        }

        pthread_mutex_lock(&client_mutex);

        switch (packet.type)
        {
            case PACKET_TYPE_SERVER_FULL:
            case PACKET_TYPE_SERVER_GAME_END:
                game->running = false;
                close(game->server_socket_fd);
                pthread_mutex_unlock(&client_mutex);
                return NULL;

            case PACKET_TYPE_SERVER_HANDSHAKE:
                game->server_pid = packet.server_handshake.pid;
                entity->spawn_point = packet.server_handshake.spawn_point;
                entity->position = packet.server_handshake.spawn_point;
                entity->number = packet.server_handshake.number;
                game->turns = packet.server_handshake.turns;
                break;

            case PACKET_TYPE_SERVER_GAME_DATA:
                entity->position = packet.server_game_data.position;
                *chunk = packet.server_game_data.chunk;
                entity->deaths = packet.server_game_data.deaths;
                entity->carried_coins = packet.server_game_data.carried_coins;
                entity->brought_coins = packet.server_game_data.brought_coins;
                game->campsite = map_chunk_get_campsite(chunk);

                entity->direction = NONE;
                break;

            default:
                break;
        }

        draw_client_interface(interface, chunk, game, entity);

        pthread_mutex_unlock(&client_mutex);
    }

    return NULL;
}

void client_main_loop(int sock_fd)
{
    double delta_time = TIME_PER_TURN;
    struct timeval last_update;
    gettimeofday(&last_update, NULL);

    MAP_CHUNK chunk;
    ENTITY entity;
    entity_init(&entity);
    entity.pid = getpid();
    entity.type = ENTITY_TYPE_PLAYER;

    GAME game;
    game_init(&game);
    game.turns = 0;
    game.server_socket_fd = sock_fd;

    send_client_handshake(game.server_socket_fd, &entity);

    INTERFACE *interface = NULL;
    if (entity.type == ENTITY_TYPE_PLAYER)
    {
        interface = interface_init();
    }

    CLIENT_LISTENER_ARGS args;
    args.game = &game;
    args.entity = &entity;
    args.chunk = &chunk;
    args.interface = interface;
    pthread_t listener;
    pthread_create(&listener, NULL, listener_thread, &args);

    while (game.running)
    {
        int key = getch();
        on_key_pressed(key, &game, &entity);

        if (game.running == false)
        {
            break;
        }

        send_client_move(game.server_socket_fd, &entity);

        update_timer(&delta_time, &last_update);

        if (delta_time < TIME_PER_TURN)
        {
            continue;
        }

        delta_time -= TIME_PER_TURN;
        game.turns++;
    }

    pthread_cancel(listener);

    interface_destroy(interface);
}
