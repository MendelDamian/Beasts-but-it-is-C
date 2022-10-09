#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "conf.h"
#include "server.h"
#include "game.h"
#include "timer.h"
#include "keyboard_handler.h"

typedef struct on_key_pressed_args_t
{
    bool *running;
} ON_KEY_PRESSED_ARGS;

void server_init(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    server->number_of_players = 0;
    memset(server->players, 0, sizeof(server->players));
}

void on_key_pressed(char key, void *arguments)
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

void server_main_loop(int server_socket_fd)
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

    SERVER server;
    server_init(&server);

    GAME game;
    game_init(&game);
    game.server_pid = getpid();
    game.server_socket_fd = server_socket_fd;
    if (map_load(&game.map, "assets/map.txt"))
    {
        return;
    }

    pthread_t timer_thread;
    TIMER_ARGS timer_args = { &delta_time, &running };
    pthread_create(&timer_thread, NULL, timer, (void *)&timer_args);

    pthread_t keyboard_handler_thread;
    ON_KEY_PRESSED_ARGS on_key_pressed_args = { &running };
    KEYBOARD_HANDLER_ARGS keyboard_handler_args = { &on_key_pressed, &on_key_pressed_args, &running };
    pthread_create(&keyboard_handler_thread, NULL, &keyboard_handler, (void *)&keyboard_handler_args);

    attron(COLOR_PAIR(PAIR_DEFAULT));

    while (running)
    {
        mvaddstr(3, 60, "Server's PID: "); printw("%d", game.server_pid);
        mvaddstr(4, 61, "Campsite X/Y: "); printw("%hhu/%hhu", 23, 11);
        mvaddstr(5, 61, "Round number: "); printw("%u", game.turns);
        mvaddstr(6, 61, "Delta time: "); printw("%4.lf", delta_time);

        map_draw(&game.map, 5, 2);

        if (delta_time < TIME_PER_TURN)
        {
            continue;
        }

        delta_time -= TIME_PER_TURN;
        game.turns++;
    }

    attroff(COLOR_PAIR(PAIR_DEFAULT));

    pthread_join(timer_thread, NULL);
    pthread_join(keyboard_handler_thread, NULL);

    endwin();
}

void player_move(PLAYER *player, DIRECTION direction, MAP *map)
{
    if (player == NULL)
    {
        return;
    }

    player->direction = direction;

    switch (direction)
    {
        case NORTH:
            if (map->tiles[player->position.y - 1][player->position.x] != TILE_WALL)
            {
                player->position.y--;
            }
            break;

        case SOUTH:
            if (map->tiles[player->position.y + 1][player->position.x] != TILE_WALL)
            {
                player->position.y++;
            }
            break;

        case WEST:
            if (map->tiles[player->position.y][player->position.x - 1] != TILE_WALL)
            {
                player->position.x--;
            }
            break;

        case EAST:
            if (map->tiles[player->position.y][player->position.x + 1] != TILE_WALL)
            {
                player->position.x++;
            }
            break;

        default:
        case NONE:
            break;
    }
}
