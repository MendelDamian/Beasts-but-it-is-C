#include "server.h"

#include <ncurses.h>
#include <unistd.h>

void server_init(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    server->number_of_players = 0;
    map_init(&server->map);
    server->turns = 0;
}

void *server_main_loop(void)
{
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
    timeout(500);  // TODO: Replace with real timer.

    colors_init();
    player_init(&server.players[0], ++server.number_of_players, (COORDS){ 9, 5 }, HUMAN);

    attron(COLOR_PAIR(PAIR_DEFAULT));
    for(;;)
    {
        int ch = getch();
        if (ch == 'q' || ch == 'Q')
        {
            break;
        }

        map_draw(&server.map, 5, 2);

        for (int i = 0; i < server.number_of_players; i++)
        {
            player_draw(&server.players[i]);
        }


        mvaddstr(3, 60, "Turns: "); printw("%d", server.turns);



        server.turns++;
    }
    attroff(COLOR_PAIR(PAIR_DEFAULT));
    endwin();

    return NULL;
}
