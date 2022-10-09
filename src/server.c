#include "server.h"

#include <unistd.h>
#include <ncurses.h>
#include <sys/time.h>

static bool running = true;

void server_init(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    server->pid = getpid();
    server->number_of_players = 0;
    map_init(&server->map);
    server->turns = 0;
}

void *server_main_loop(void)
{
    struct timeval last_update, now;

    double delta_time = TIME_PER_TURN;

    SERVER server;
    server_init(&server);
    map_init(&server.map);
    if (map_load(&server.map, "assets/map.txt"))
    {
        return NULL;
    }

    start_color();

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(0);
    refresh();
    timeout(1);

    colors_init();
    player_init(&server.players[0], ++server.number_of_players, (COORDS){ 9, 5 }, HUMAN);

    attron(COLOR_PAIR(PAIR_DEFAULT));

    gettimeofday(&last_update, NULL);
    while (running)
    {
        gettimeofday(&now, NULL);
        delta_time += ((now.tv_sec - last_update.tv_sec) * 1000) + ((now.tv_usec - last_update.tv_usec) / 1000);
        last_update = now;

        mvaddstr(3, 60, "Server's PID: "); printw("%d", server.pid);
        mvaddstr(4, 61, "Campsite X/Y: "); printw("%hhu/%hhu", 23, 11);
        mvaddstr(5, 61, "Round number: "); printw("%u", server.turns);
        mvaddstr(6, 61, "Delta time: "); printw("%4.lf", delta_time);

        handle_keys();

        if (delta_time < TIME_PER_TURN)
        {
            continue;
        }

        map_draw(&server.map, 5, 2);

        for (int i = 0; i < server.number_of_players; i++)
        {
            player_draw(&server.players[i]);
        }

        delta_time -= TIME_PER_TURN;
        server.turns++;
    }
    attroff(COLOR_PAIR(PAIR_DEFAULT));
    endwin();

    return NULL;
}

void handle_keys(void)
{
    int ch = getch();
    switch (ch)
    {
        case 'q':
        case 'Q':
            running = false;
            break;

        default:
            break;
    }
}
