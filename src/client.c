#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "game.h"
#include "player.h"
#include "timer.h"
#include "packets.h"
#include "screen.h"

static void on_key_pressed(char key, bool *running, PLAYER *player)
{
    if (running == NULL || player == NULL)
    {
        return;
    }

    switch (key)
    {
        case 'Q':
        case 'q':
            *running = false;
            break;

        case 'W':
        case 'w':
            player->direction = NORTH;
            break;

        case 'A':
        case 'a':
            player->direction = WEST;
            break;

        case 'S':
        case 's':
            player->direction = SOUTH;
            break;

        case 'D':
        case 'd':
            player->direction = EAST;
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
    struct timeval last_update;
    gettimeofday(&last_update, NULL);

    screen_init();

    GAME game;
    game_init(&game);
    game.turns = 0;
    game.server_socket_fd = sock_fd;

    MAP_CHUNK chunk;
    PLAYER player;
    player.pid = getpid();

    while (running)
    {
        char key = getch();
        on_key_pressed(key, &running, &player);
        update_timer(&delta_time, &last_update);

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

    endwin();
}
