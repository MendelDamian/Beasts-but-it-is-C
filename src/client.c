#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#include "client.h"
#include "game.h"
#include "player.h"
#include "timer.h"
#include "keyboard_handler.h"
#include "packets.h"
#include "screen.h"

typedef struct on_key_pressed_args_t
{
    bool *running;
    PLAYER *player;
} ON_KEY_PRESSED_ARGS;

static void on_key_pressed(char key, void *arguments)
{
    if (arguments == NULL)
    {
        return;
    }

    ON_KEY_PRESSED_ARGS args = *(ON_KEY_PRESSED_ARGS *)arguments;

    if (args.running == NULL || args.player == NULL)
    {
        return;
    }

    switch (key)
    {
        case 'q':
            *args.running = false;
            break;

        case 'w':
            args.player->direction = NORTH;
            break;

        case 'a':
            args.player->direction = WEST;
            break;

        case 's':
            args.player->direction = SOUTH;
            break;

        case 'd':
            args.player->direction = EAST;
            break;

        default:
            break;
    }
}

void send_packet(bool *running, PLAYER *player, GAME *game, MAP_CHUNK *chunk)
{
    PACKET_DATA packet_data = {
        .pid = player->pid,
        .direction = player->direction,
        .quit = false,
    };

    if (send(game->server_socket_fd, &packet_data, sizeof(packet_data), 0) == -1)
    {
        return;
    }
    player->direction = NONE;

    PACKET_DATA_RESPONSE packet_data_response;
    if (recv(game->server_socket_fd, &packet_data_response, sizeof(packet_data_response), 0) == -1)
    {
        return;
    }

    if (packet_data_response.end)
    {
        *running = false;
    }

    game->server_pid = packet_data_response.pid;
    player->number = packet_data_response.number;
    *chunk = packet_data_response.chunk;
    game->turns = packet_data_response.turns;
    player->carried_coins = packet_data_response.carried_coins;
    player->brought_coins = packet_data_response.brought_coins;
    player->deaths = packet_data_response.deaths;
    player->position = packet_data_response.position;
    player->state = packet_data_response.state;
}

void client_main_loop(int sock_fd)
{
    double delta_time = TIME_PER_TURN;
    bool running = true;

    screen_init();

    GAME game;
    game_init(&game);
    game.turns = 0;
    game.server_socket_fd = sock_fd;

    MAP_CHUNK chunk;

    PLAYER player;
    player.pid = getpid();

    pthread_t timer_thread;
    TIMER_ARGS timer_args = { &delta_time, &running };
    pthread_create(&timer_thread, NULL, timer, (void *)&timer_args);

    pthread_t keyboard_handler_thread;
    ON_KEY_PRESSED_ARGS on_key_pressed_args = { &running, &player };
    KEYBOARD_HANDLER_ARGS keyboard_handler_args = { &on_key_pressed, &on_key_pressed_args, &running };
    pthread_create(&keyboard_handler_thread, NULL, keyboard_handler, (void *)&keyboard_handler_args);

    while (running)
    {
        if (delta_time < TIME_PER_TURN)
        {
            continue;
        }

        if (player.direction != NONE)
        {
            clear();
        }

        send_packet(&running, &player, &game, &chunk);

        draw_client_interface(&chunk, &game, &player);

        delta_time -= TIME_PER_TURN;
        game.turns++;
    }

    pthread_join(timer_thread, NULL);
    pthread_join(keyboard_handler_thread, NULL);

    endwin();
}
