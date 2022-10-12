#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "conf.h"
#include "server.h"
#include "game.h"
#include "timer.h"
#include "keyboard_handler.h"
#include "coordinate.h"
#include "packets.h"
#include "screen.h"

typedef struct on_key_pressed_args_t
{
    bool *running;
} ON_KEY_PRESSED_ARGS;

typedef struct server_acceptance_args_t
{
    bool *running;
    GAME *game;
    SERVER *server;
} SERVER_ACCEPTANCE_ARGS;

typedef struct server_player_handler_args_t
{
    bool *running;
    GAME *game;
    SERVER *server;
    PLAYER *player;
} SERVER_PLAYER_HANDLER_ARGS;

void server_init(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    server->number_of_players = 0;
    memset(server->players, 0, sizeof(server->players));
}

PLAYER *server_add_player(SERVER *server)
{
    if (server == NULL)
    {
        return NULL;
    }

    if (server->number_of_players >= MAX_PLAYERS)
    {
        return NULL;
    }

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (server->players[i].pid == 0)
        {
            server->number_of_players++;
            server->players[i].number = i + 1;
            return &server->players[i];
        }
    }

    return NULL;
}

void server_remove_player(SERVER *server, int pid)
{
    if (server == NULL)
    {
        return;
    }

    for (int i = 0; i < server->number_of_players; i++)
    {
        if (server->players[i].pid == pid)
        {
            memset(&server->players[i], 0, sizeof(PLAYER));
            server->number_of_players--;
            break;
        }
    }
}

COORDS find_spot_for_player(MAP *map, SERVER *server)
{
    if (map == NULL || server == NULL)
    {
        return (COORDS){-1, -1};
    }

    while (1)
    {
        int x = rand() % map->width;
        int y = rand() % map->height;

        if (map->tiles[y][x] == TILE_EMPTY)
        {
            bool found = false;
            for (int i = 0; i < server->number_of_players; i++)
            {
                if (server->players[i].position.x == x && server->players[i].position.y == y)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                return (COORDS){y, x};
            }
        }
    }
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

void *server_player_handler(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    SERVER_PLAYER_HANDLER_ARGS args = *(SERVER_PLAYER_HANDLER_ARGS *)arguments;
    if (args.running == NULL || args.game == NULL || args.server == NULL || args.player == NULL)
    {
        return NULL;
    }

    while (*args.running)
    {
        PACKET_DATA packet_data;
        ssize_t bytes_received = recv(args.player->socket_fd, &packet_data, sizeof(PACKET_DATA), 0);
        if (bytes_received <= 0)
        {
            int socket_fd = args.player->socket_fd;
            server_remove_player(args.server, args.player->pid);
            close(socket_fd);
            break;
        }

        args.player->pid = packet_data.pid;
        player_move(args.player, packet_data.direction, &args.server->map);

        MAP_CHUNK chunk;
        map_get_chunk(&args.server->map, &chunk, args.player->position);
        PACKET_DATA_RESPONSE packet_data_response = {
            .pid = args.game->server_pid,
            .number = args.player->number,
            .chunk = chunk,
            .turns = args.game->turns,
            .carried_coins = args.player->carried_coins,
            .brought_coins = args.player->brought_coins,
            .deaths = args.player->deaths,
            .position = args.player->position,
            .state = args.player->state,
            .end = false,
        };

        send(args.player->socket_fd, (void *)&packet_data_response, sizeof(PACKET_DATA_RESPONSE), 0);
    }

    return NULL;
}

void *server_acceptance(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    SERVER_ACCEPTANCE_ARGS args = *(SERVER_ACCEPTANCE_ARGS *)arguments;
    if (args.running == NULL || args.game == NULL || args.server == NULL)
    {
        return NULL;
    }

    while (*args.running)
    {
        int client_socket = accept(args.game->server_socket_fd,NULL, NULL);
        if (client_socket == -1)
        {
            continue;
        }

        PLAYER *player = server_add_player(args.server);
        if (player == NULL)
        {
            close(client_socket);
            continue;
        }

        player_init(player, find_spot_for_player(&args.server->map, args.server), HUMAN);
        player->socket_fd = client_socket;

        SERVER_PLAYER_HANDLER_ARGS player_handler_args = {
            .running = args.running,
            .game = args.game,
            .server = args.server,
            .player = player,
        };
        pthread_create(&player->thread_id, NULL, server_player_handler, (void *)&player_handler_args);
    }

    return NULL;
}

void server_main_loop(int server_socket_fd)
{
    double delta_time = TIME_PER_TURN;
    bool running = true;

    screen_init();

    SERVER server;
    server_init(&server);

    GAME game;
    game_init(&game);
    game.server_pid = getpid();
    game.server_socket_fd = server_socket_fd;

    if (map_load(&server.map, MAP_FILE))
    {
        perror("ERROR on map_load");
        return;
    }

    pthread_t timer_thread;
    TIMER_ARGS timer_args = { &delta_time, &running };
    pthread_create(&timer_thread, NULL, timer, (void *)&timer_args);

    pthread_t keyboard_handler_thread;
    ON_KEY_PRESSED_ARGS on_key_pressed_args = { &running };
    KEYBOARD_HANDLER_ARGS keyboard_handler_args = {
            &on_key_pressed,
            &on_key_pressed_args,
            &running
    };
    pthread_create(&keyboard_handler_thread, NULL, keyboard_handler, (void *)&keyboard_handler_args);

    pthread_t server_acceptance_thread;
    SERVER_ACCEPTANCE_ARGS server_acceptance_args = {
        .running = &running,
        .game = &game,
        .server = &server,
    };
    pthread_create(&server_acceptance_thread, NULL, server_acceptance, (void *)&server_acceptance_args);
    pthread_detach(server_acceptance_thread);

    attron(COLOR_PAIR(PAIR_DEFAULT));

    while (running)
    {
        if (delta_time < TIME_PER_TURN)
        {
            continue;
        }

        draw_server_interface(&server, &game);

        delta_time -= TIME_PER_TURN;
        game.turns++;
    }

    attroff(COLOR_PAIR(PAIR_DEFAULT));

    pthread_join(timer_thread, NULL);
    pthread_join(keyboard_handler_thread, NULL);
    pthread_join(server_acceptance_thread, NULL);

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
