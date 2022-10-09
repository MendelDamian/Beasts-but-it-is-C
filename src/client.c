#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "client.h"
#include "game.h"
#include "player.h"
#include "timer.h"
#include "keyboard_handler.h"

typedef struct on_key_pressed_args_t
{
    bool *running;
} ON_KEY_PRESSED_ARGS;

static void on_key_pressed(char key, void *arguments)
{
    if (arguments == NULL)
    {
        return;
    }

    ON_KEY_PRESSED_ARGS *args = (ON_KEY_PRESSED_ARGS *)arguments;

    if (args->running == NULL)
    {
        return;
    }

    if (key == 'q' || key == 'Q')
    {
        *args->running = false;
    }
}

void main_loop(int sock_fd)
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

    PLAYER player;
    player_init(&player, 1, (COORDS){ 9, 5 }, HUMAN);

    GAME game;
    game.turns = 0;
    game.server_socket_fd = sock_fd;
    map_init(&game.map);

    pthread_t timer_thread;
    TIMER_ARGS timer_args = { &delta_time, &running };
    pthread_create(&timer_thread, NULL, timer, (void *)&timer_args);

    pthread_t keyboard_handler_thread;
    ON_KEY_PRESSED_ARGS on_key_pressed_args = { &running };
    KEYBOARD_HANDLER_ARGS keyboard_handler_args = { &on_key_pressed, &on_key_pressed_args, &running };
    pthread_create(&keyboard_handler_thread, NULL, keyboard_handler, (void *)&keyboard_handler_args);

    while (running)
    {
        mvaddstr(3, 60, "Server's PID: "); printw("%d", 1);
        mvaddstr(4, 61, "Campsite X/Y: "); printw("%s", "unknown");
        mvaddstr(5, 61, "Round number: "); printw("%u", game.turns);

        mvaddstr(7, 60, "Player:");
        mvaddstr(8, 61, "Number: "); printw("%hhu", player.number);
        mvaddstr(9, 61, "Type: "); printw("%s", "HUMAN");
        mvaddstr(10, 61, "Curr X/Y: "); printw("%02hhu/%02hhu", player.position.x, player.position.y);
        mvaddstr(11, 61, "Deaths: "); printw("%hu", player.deaths);

        if (delta_time < TIME_PER_TURN)
        {
            continue;
        }

        map_draw(&game.map, 5, 2);

        delta_time -= TIME_PER_TURN;
        game.turns++;
    }

    pthread_join(timer_thread, NULL);
    pthread_join(keyboard_handler_thread, NULL);

    endwin();
}
