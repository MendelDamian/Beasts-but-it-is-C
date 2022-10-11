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

void client_main_loop(int sock_fd)
{
    bool running = true;
    double delta_time = TIME_PER_TURN;

    initscr();
    start_color();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(0);
    refresh();
    timeout(0);

    colors_init();

    GAME game;
    game.turns = 0;
    game.server_socket_fd = sock_fd;
    map_init(&game.map);

    PLAYER player;

    int pid = getpid();
    PACKET_DATA packet_data = {
        .pid = pid,
        .direction = NONE,
        .quit = false,
    };
    send(sock_fd, &packet_data, sizeof(PACKET_DATA), 0);

    PACKET_DATA_RESPONSE packet_data_response;
    recv(sock_fd, &packet_data_response, sizeof(PACKET_DATA_RESPONSE), 0);
    map_draw_chunk(&packet_data_response.chunk, 5, 2);
    game.server_pid = packet_data_response.pid;
    game.turns = packet_data_response.turns;
    player_init(&player, packet_data_response.position, HUMAN);
    player.number = 1;
    player.carried_coins = packet_data_response.carried_coins;
    player.brought_coins = packet_data_response.brought_coins;
    player.deaths = packet_data_response.deaths;
    player.state = packet_data_response.state;

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
        clear();

        mvaddstr(3, 60, "Server's PID: "); printw("%d", 1);
        mvaddstr(4, 61, "Campsite X/Y: "); printw("%s", "unknown");
        mvaddstr(5, 61, "Round number: "); printw("%u", game.turns);

        mvaddstr(7, 60, "Player:");
        mvaddstr(8, 61, "Number: "); printw("%hhu", player.number);
        mvaddstr(9, 61, "Type: "); printw("%s", "HUMAN");
        mvaddstr(10, 61, "Curr X/Y: "); printw("%02hhu/%02hhu", player.position.x, player.position.y);
        mvaddstr(11, 61, "Deaths: "); printw("%hu", player.deaths);

        packet_data.pid = pid;
        packet_data.direction = player.direction;
        packet_data.quit = false;

        send(sock_fd, &packet_data, sizeof(PACKET_DATA), 0);
        player.direction = NONE;
        recv(sock_fd, &packet_data_response, sizeof(PACKET_DATA_RESPONSE), 0);

        map_draw_chunk(&packet_data_response.chunk, 5, 2);
        player.carried_coins = packet_data_response.carried_coins;
        player.brought_coins = packet_data_response.brought_coins;
        player.deaths = packet_data_response.deaths;
        player.state = packet_data_response.state;
        player.position = packet_data_response.position;

        player_draw(&player, 5, 2);

        delta_time -= TIME_PER_TURN;
        game.turns++;
    }

    pthread_join(timer_thread, NULL);
    pthread_join(keyboard_handler_thread, NULL);

    endwin();
}
