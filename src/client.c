#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "client.h"
#include "game.h"
#include "player.h"
#include "timer.h"

void main_loop(int *sock_fd)
{
    if (sock_fd == NULL)
    {
        return;
    }

    initscr();
    start_color();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(0);
    refresh();
    timeout(1);

    double delta_time = TIME_PER_TURN;
    bool running = true;
    char pressed_key = -1;

    PLAYER player;
    player_init(&player, 1, (COORDS){ 9, 5 }, HUMAN);

    GAME game;
    game.turns = 0;
    game.server_socket_fd = sock_fd;
    map_init(&game.map);

    pthread_t timer_thread;
    TIMER_ARGS timer_args = { &delta_time, &running, &pressed_key };
    pthread_create(&timer_thread, NULL, timer, (void *)&timer_args);

    while (running)
    {
        map_draw(&game.map, 5, 2);

        if (pressed_key == 'q' || pressed_key == 'Q')
        {
            running = false;
        }
    }

    pthread_join(timer_thread, NULL);
    endwin();
}
