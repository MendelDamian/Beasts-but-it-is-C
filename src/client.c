#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#include "network_protocol.h"
#include "client.h"
#include "game.h"
#include "timer.h"
#include "screen.h"

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct client_listener_args_t
{
    GAME *game;
    ENTITY *entity;
    MAP_CHUNK *chunk;
} CLIENT_LISTENER_ARGS;

static void on_key_pressed(char key, GAME *game, ENTITY *entity)
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
            entity->direction = NORTH;
            break;

        case 'A':
        case 'a':
            entity->direction = WEST;
            break;

        case 'S':
        case 's':
            entity->direction = SOUTH;
            break;

        case 'D':
        case 'd':
            entity->direction = EAST;
            break;

        default:
            break;
    }
}

void *client_listener(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    CLIENT_LISTENER_ARGS args = *(CLIENT_LISTENER_ARGS *)arguments;
    if (args.game == NULL || args.entity == NULL || args.chunk == NULL)
    {
        return NULL;
    }

    GAME *game = args.game;
    ENTITY *entity = args.entity;
    MAP_CHUNK *chunk = args.chunk;

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
            case PACKET_TYPE_SERVER_GAME_END:
            case PACKET_TYPE_SERVER_FULL:
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

                entity->direction = NONE;
                break;

            default:
                break;
        }

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
    entity.pid = getpid();

    GAME game;
    game_init(&game);
    game.turns = 0;
    game.server_socket_fd = sock_fd;

    send_client_handshake(game.server_socket_fd, &entity);

    screen_init();

    CLIENT_LISTENER_ARGS args;
    args.game = &game;
    args.entity = &entity;
    args.chunk = &chunk;
    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, client_listener, &args);

    while (game.running)
    {
        char key = getch();
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

        pthread_mutex_lock(&client_mutex);
        clear();
        draw_client_interface(&chunk, &game, &entity);
        pthread_mutex_unlock(&client_mutex);

        delta_time -= TIME_PER_TURN;
        game.turns++;
    }

    pthread_join(listener_thread, NULL);

    endwin();
}
