#define _DEFAULT_SOURCE  // To clear warnings about usleep() implicit declaration.
#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdbool.h>

#include "server.h"
#include "network_protocol.h"
#include "timer.h"
#include "interface.h"

pthread_mutex_t game_state_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct entity_thread_args
{
    ENTITY *entity;
    SERVER *server;
} ENTITY_THREAD_ARGS;

static void init(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    server->number_of_players = 0;
    server->number_of_beasts = 0;
    server->entities = dll_create(sizeof(ENTITY));
    server->dropped_treasures = dll_create(sizeof(TREASURE));
    map_init(&server->map);
    game_init(&server->game);
    server->game.campsite = (COORDS){ 0, 0 };
}

static void destroy(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    NODE *node = server->entities->head;
    while (node != NULL)
    {
        ENTITY *entity = (ENTITY *)node->item;
        pthread_cancel(entity->thread);
        node = node->next;
    }

    dll_clear(server->entities);
    dll_clear(server->dropped_treasures);
    free(server->entities);
    free(server->dropped_treasures);
}

static void prepare_map_chunk(SERVER *server, MAP_CHUNK *chunk)
{
    if (server == NULL || chunk == NULL)
    {
        return;
    }

    uint8_t begin_x = chunk->position.x - chunk->width / 2;
    uint8_t begin_y = chunk->position.y - chunk->height / 2;

    uint8_t end_x = chunk->position.x + chunk->width / 2;
    uint8_t end_y = chunk->position.y + chunk->height / 2;

    NODE *node = server->entities->head;
    while (node)
    {
        ENTITY *entity = node->item;
        if (entity->position.x >= begin_x && entity->position.x <= end_x
            && entity->position.y >= begin_y && entity->position.y <= end_y)
        {
            uint8_t y = entity->position.y - begin_y;
            uint8_t x = entity->position.x - begin_x;

            switch (entity->type)
            {
                case ENTITY_TYPE_PLAYER:
                case ENTITY_TYPE_BOT:
                    chunk->tiles[y][x] = (char)(entity->number + '0');
                    break;

                case ENTITY_TYPE_BEAST:
                    chunk->tiles[y][x] = TILE_BEAST;
                    break;

                default:
                    break;
            }
        }
        node = node->next;
    }


    node = server->dropped_treasures->head;
    while (node)
    {
        TREASURE *treasure = (TREASURE *)node->item;
        if (treasure->position.x >= begin_x && treasure->position.x <= end_x
            && treasure->position.y >= begin_y && treasure->position.y <= end_y)
        {
            uint8_t y = treasure->position.y - begin_y;
            uint8_t x = treasure->position.x - begin_x;
            chunk->tiles[y][x] = TILE_DROPPED_TREASURE;
        }
        node = node->next;
    }
}

static COORDS map_find_free_tile(SERVER *server)
{
    COORDS coords = {0, 0};
    if (server == NULL)
    {
        return coords;
    }

    MAP *map = &server->map;

    // Check if there is any free tile.
    bool free_tile_found = false;
    for (uint8_t i = 0; i < map->height; ++i)
    {
        for (uint8_t j = 0; j < map->width; ++j)
        {
            if (map->tiles[i][j] == TILE_EMPTY
                && get_entity_at_coords(server->entities, (COORDS){ j, i }, NULL) == NULL
                && get_entity_at_spawn_point(server->entities, (COORDS){ j, i }) == NULL
                && get_treasure_at_coords(server->dropped_treasures, (COORDS){ j, i }) == NULL)
            {
                free_tile_found = true;
                break;
            }
        }
        if (free_tile_found)
        {
            break;
        }
    }

    if (free_tile_found == false)
    {
        return coords;
    }

    do
    {
        coords.x = rand() % map->width;
        coords.y = rand() % map->height;
    } while (map->tiles[coords.y][coords.x] != TILE_EMPTY);

    return coords;
}

static ENTITY *add_entity(SERVER *server)
{
    if (server == NULL)
    {
        return NULL;
    }

    return dll_push_back(server->entities);
}

static void remove_entity(SERVER *server, ENTITY *entity)
{
    if (server == NULL || entity == NULL)
    {
        return;
    }

    switch (entity->type)
    {
        case ENTITY_TYPE_BOT:
        case ENTITY_TYPE_PLAYER:
            server->number_of_players--;
            break;

        case ENTITY_TYPE_BEAST:
            server->number_of_beasts--;
            break;

        default:
            break;
    }

    dll_remove(server->entities, entity);
}

static TREASURE *add_treasure(SERVER *server, COORDS coords, uint16_t coins)
{
    if (server == NULL)
    {
        return NULL;
    }

    TREASURE *treasure = dll_push_back(server->dropped_treasures);
    treasure->position = coords;
    treasure->coins = coins;
    return treasure;
}

static void remove_treasure(SERVER *server, TREASURE *treasure)
{
    if (server == NULL || treasure == NULL)
    {
        return;
    }

    dll_remove(server->dropped_treasures, treasure);
}

static uint8_t player_get_number(DLL *entities)
{
    if (entities == NULL)
    {
        return 0;
    }

    uint8_t number = 1;
    NODE *node = entities->head;
    while (node != NULL)
    {
        ENTITY *entity = node->item;
        if (entity->number == number)
        {
            number++;
            node = entities->head;
        }
        else
        {
            node = node->next;
        }
    }

    return number;
}

static void spawn_cash(SERVER *server, TILE type)
{
    if (server == NULL)
    {
        return;
    }

    if (type != TILE_COIN && type != TILE_TREASURE && type != TILE_LARGE_TREASURE)
    {
        return;
    }

    COORDS coords = map_find_free_tile(server);
    if (coords.x == 0 && coords.y == 0)
    {
        return;
    }

    pthread_mutex_lock(&game_state_mutex);
    server->map.tiles[coords.y][coords.x] = type;
    pthread_mutex_unlock(&game_state_mutex);
}

static void add_beast(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    if (server->number_of_beasts >= MAX_BEASTS)
    {
        return;
    }

    pthread_mutex_lock(&game_state_mutex);

    COORDS coords = map_find_free_tile(server);
    if (coords.x == 0 && coords.y == 0)
    {
        return;
    }

    server->number_of_beasts++;

    ENTITY *beast = add_entity(server);
    beast->type = ENTITY_TYPE_BEAST;
    beast->position = coords;
    beast->spawn_point = coords;

    pthread_mutex_unlock(&game_state_mutex);

    ENTITY_THREAD_ARGS *args = malloc(sizeof(ENTITY_THREAD_ARGS));
    args->server = server;
    args->entity = beast;
    pthread_create(&beast->thread, NULL, beast_thread, args);
}

static void on_key_pressed(int key, SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    switch (key)
    {
        case 'Q':
        case 'q':
            server->game.running = false;
            break;

        case TILE_COIN:
            spawn_cash(server, TILE_COIN);
            break;

        case TILE_TREASURE:
            spawn_cash(server, TILE_TREASURE);
            break;

        case TILE_LARGE_TREASURE:
            spawn_cash(server, TILE_LARGE_TREASURE);
            break;

        case 'B':
        case 'b':
            add_beast(server);
            break;

        default:
            break;
    }
}

static void handle_game_state(SERVER *server)
{
    if (server == NULL)
    {
        return;
    }

    // To prevent the game from de-synchronization, we need to divide the game state handle into two phases:
    // 1. Update the entities positions
    // 2. Update the game state itself

    // Phase 1. Update the entities positions.
    NODE *node = server->entities->head;
    while (node)
    {
        ENTITY *entity = node->item;

        if (server->map.tiles[entity->position.y][entity->position.x] == TILE_BUSH && entity->stagnancy == 0)
        {
            // Bushes slow down the entity.
            entity->stagnancy++;
        }
        else
        {
            // Move the entity.
            bool moved = false;
            switch (entity->direction)
            {
                case NORTH:
                    if (server->map.tiles[entity->position.y - 1][entity->position.x] != TILE_WALL)
                    {
                        moved = true;
                        entity->position.y--;
                    }
                    break;

                case SOUTH:
                    if (server->map.tiles[entity->position.y + 1][entity->position.x] != TILE_WALL)
                    {
                        moved = true;
                        entity->position.y++;
                    }
                    break;

                case WEST:
                    if (server->map.tiles[entity->position.y][entity->position.x - 1] != TILE_WALL)
                    {
                        moved = true;
                        entity->position.x--;
                    }
                    break;

                case EAST:
                    if (server->map.tiles[entity->position.y][entity->position.x + 1] != TILE_WALL)
                    {
                        moved = true;
                        entity->position.x++;
                    }
                    break;

                default:
                    break;
            }

            if (moved)
            {
                entity->stagnancy = 0;
            } else
            {
                entity->stagnancy++;
            }
        }

        node = node->next;
    }

    // Phase 2. Update the game state.
    node = server->entities->head;
    while (node)
    {
        ENTITY *entity = node->item;
        TREASURE *treasure = NULL;
        if (entity->type == ENTITY_TYPE_BEAST)
        {
            node = node->next;
            continue;
        }

        // Check if the entity shares tile with any other entity.
        ENTITY *other_entity = get_entity_at_coords(server->entities, entity->position, entity);
        if (other_entity != NULL)
        {
            COORDS meeting_point = entity->position;
            uint32_t total_coins = entity->carried_coins + other_entity->carried_coins;

            entity->position = entity->spawn_point;
            entity->deaths++;

            if (other_entity->type != ENTITY_TYPE_BEAST)
            {
                other_entity->deaths++;
                other_entity->position = other_entity->spawn_point;
            }

            // If the entities are both beasts, treasure is not spawned.
            entity->carried_coins = 0;
            other_entity->carried_coins = 0;
            add_treasure(server, meeting_point, total_coins);
        }
        // Check for dropped treasures.
        else if ((treasure = get_treasure_at_coords(server->dropped_treasures, entity->position)) != NULL)
        {
            entity->carried_coins += treasure->coins;
            remove_treasure(server, treasure);
        }
        // Check if non-beast entity stands on specific block.
        else
        {
            char stands_on = server->map.tiles[entity->position.y][entity->position.x];
            switch (stands_on)
            {
                case TILE_COIN:
                    entity->carried_coins += COIN_CASH;
                    server->map.tiles[entity->position.y][entity->position.x] = TILE_EMPTY;
                    break;

                case TILE_TREASURE:
                    entity->carried_coins += TREASURE_CASH;
                    server->map.tiles[entity->position.y][entity->position.x] = TILE_EMPTY;
                    break;

                case TILE_LARGE_TREASURE:
                    entity->carried_coins += LARGE_TREASURE_CASH;
                    server->map.tiles[entity->position.y][entity->position.x] = TILE_EMPTY;
                    break;

                case TILE_CAMPSITE:
                    entity->brought_coins += entity->carried_coins;
                    entity->carried_coins = 0;
                    break;

                default:
                    break;
            }
        }

        node = node->next;
    }

    // Send the game state to the clients.
    node = server->entities->head;
    while (node)
    {
        ENTITY *entity = node->item;
        if (entity->type != ENTITY_TYPE_BEAST)
        {
            MAP_CHUNK chunk;
            map_get_chunk(&server->map, &chunk, entity->position);
            prepare_map_chunk(server, &chunk);
            send_server_game_data(entity->socket_fd, entity, &chunk);
        }

        node = node->next;
    }
}

static void *entity_thread(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    ENTITY_THREAD_ARGS args = *(ENTITY_THREAD_ARGS *)arguments;
    free(arguments);

    if (args.server == NULL || args.entity == NULL)
    {
        return NULL;
    }

    SERVER *server = args.server;
    ENTITY *entity = args.entity;

    while (server->game.running)
    {
        PACKET packet;
        ssize_t received_bytes = recv_packet(entity->socket_fd, &packet);
        pthread_mutex_lock(&game_state_mutex);
        if (received_bytes <= 0)
        {
            // Client has disconnected or something went wrong.
            close(entity->socket_fd);
            remove_entity(server, entity);
            pthread_mutex_unlock(&game_state_mutex);
            pthread_exit(NULL);
        }

        switch (packet.type)
        {
            case PACKET_TYPE_CLIENT_MOVE:
                entity->direction = packet.client_move.direction;
                break;

            case PACKET_TYPE_CLIENT_QUIT:
                close(entity->socket_fd);
                remove_entity(server, entity);
                pthread_mutex_unlock(&game_state_mutex);
                pthread_exit(NULL);

            default:
                break;
        }

        pthread_mutex_unlock(&game_state_mutex);
    }

    return NULL;
}

static void *acceptance_thread(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    SERVER *server = (SERVER *)arguments;
    if (server == NULL)
    {
        return NULL;
    }

    while (server->game.running)
    {
        int client_socket = accept(server->game.server_socket_fd,NULL, NULL);
        if (client_socket == -1)
        {
            continue;
        }

        PACKET packet;
        ssize_t received_bytes = recv_packet(client_socket, &packet);
        if (received_bytes <= 0 || packet.type != PACKET_TYPE_CLIENT_HANDSHAKE)
        {
            // Something went wrong.
            close(client_socket);
            continue;
        }

        pthread_mutex_lock(&game_state_mutex);

        ENTITY *entity = NULL;

        switch (packet.client_handshake.type)
        {
            case ENTITY_TYPE_BOT:
            case ENTITY_TYPE_PLAYER:
                if (server->number_of_players >= MAX_PLAYERS)
                {
                    // Server is full.
                    send_server_full(client_socket);
                    close(client_socket);
                    pthread_mutex_unlock(&game_state_mutex);
                    continue;
                }

                COORDS position = map_find_free_tile(server);
                if (position.x == 0 && position.y == 0)
                {
                    // Map is full.
                    send_server_game_end(client_socket);
                    close(client_socket);
                    pthread_mutex_unlock(&game_state_mutex);
                    continue;
                }

                server->number_of_players++;

                entity = add_entity(server);
                entity->type = packet.client_handshake.type;
                entity->pid = packet.client_handshake.pid;
                entity->number = player_get_number(server->entities);
                entity->spawn_point = position;
                entity->position = entity->spawn_point;

                break;

            case ENTITY_TYPE_BEAST:
            {
                break;
            }

            default:
                // Something went wrong.
                close(client_socket);
                pthread_mutex_unlock(&game_state_mutex);
                continue;
        }

        entity->socket_fd = client_socket;
        send_server_handshake(entity->socket_fd, entity, &server->game);

        ENTITY_THREAD_ARGS *args = malloc(sizeof(ENTITY_THREAD_ARGS));
        args->server = server;
        args->entity = entity;

        pthread_create(&entity->thread, NULL, entity_thread, args);

        pthread_mutex_unlock(&game_state_mutex);
    }

    return NULL;
}

static DIRECTION beast_determine_direction(SERVER *server, ENTITY *beast)
{
    if (server == NULL || beast == NULL)
    {
        return NONE;
    }

    MAP_CHUNK chunk;
    map_get_chunk(&server->map, &chunk, beast->position);
    prepare_map_chunk(server, &chunk);

    bool found = false;
    for (int i = 0; i < chunk.height; ++i)
    {
        for (int j = 0; j < chunk.width; ++j)
        {
            if (is_tile_player(chunk.tiles[i][j]))
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            break;
        }
    }

    if (!found)
    {
        return rand() % 4;
    }

    // Close eyes.
    if (is_tile_player(chunk.tiles[1][2]))
    {
        return NORTH;
    }
    if (is_tile_player(chunk.tiles[2][3]))
    {
        return EAST;
    }
    if (is_tile_player(chunk.tiles[3][2]))
    {
        return SOUTH;
    }
    if (is_tile_player(chunk.tiles[2][1]))
    {
        return WEST;
    }

    if (chunk.tiles[1][2] != TILE_WALL && is_tile_player(chunk.tiles[0][2]))
    {
        return NORTH;
    }
    if (chunk.tiles[2][3] != TILE_WALL && is_tile_player(chunk.tiles[2][4]))
    {
        return EAST;
    }
    if (chunk.tiles[3][2] != TILE_WALL && is_tile_player(chunk.tiles[4][2]))
    {
        return SOUTH;
    }
    if (chunk.tiles[2][1] != TILE_WALL && is_tile_player(chunk.tiles[2][0]))
    {
        return WEST;
    }

    if (chunk.tiles[1][2] != TILE_WALL && is_tile_player(chunk.tiles[0][1])
        && (chunk.tiles[0][2] == TILE_WALL || chunk.tiles[1][1] == TILE_WALL))
    {
        return NORTH;
    }
    if (chunk.tiles[1][2] != TILE_WALL && is_tile_player(chunk.tiles[0][3])
        && (chunk.tiles[0][2] == TILE_WALL || chunk.tiles[1][3] == TILE_WALL))
    {
        return NORTH;
    }
    if (chunk.tiles[2][3] != TILE_WALL && is_tile_player(chunk.tiles[1][4])
        && (chunk.tiles[2][4] == TILE_WALL || chunk.tiles[1][3] == TILE_WALL))
    {
        return EAST;
    }
    if (chunk.tiles[2][3] != TILE_WALL && is_tile_player(chunk.tiles[3][4])
        && (chunk.tiles[2][4] == TILE_WALL || chunk.tiles[3][3] == TILE_WALL))
    {
        return EAST;
    }
    if (chunk.tiles[3][2] != TILE_WALL && is_tile_player(chunk.tiles[4][1])
        && (chunk.tiles[4][2] == TILE_WALL || chunk.tiles[3][1] == TILE_WALL))
    {
        return SOUTH;
    }
    if (chunk.tiles[3][2] != TILE_WALL && is_tile_player(chunk.tiles[4][3])
        && (chunk.tiles[4][2] == TILE_WALL || chunk.tiles[3][3] == TILE_WALL))
    {
        return SOUTH;
    }
    if (chunk.tiles[2][1] != TILE_WALL && is_tile_player(chunk.tiles[1][0])
        && (chunk.tiles[2][0] == TILE_WALL || chunk.tiles[1][1] == TILE_WALL))
    {
        return WEST;
    }
    if (chunk.tiles[2][1] != TILE_WALL && is_tile_player(chunk.tiles[3][0])
        && (chunk.tiles[2][0] == TILE_WALL || chunk.tiles[3][1] == TILE_WALL))
    {
        return WEST;
    }

    return rand() % 4;
}

void *beast_thread(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    ENTITY_THREAD_ARGS args = *(ENTITY_THREAD_ARGS *)arguments;
    free(arguments);
    ENTITY *entity = args.entity;
    SERVER *server = args.server;

    while (server->game.running)
    {
        usleep(200000);  // Sleep for 200ms.

        pthread_mutex_lock(&game_state_mutex);
        entity->direction = beast_determine_direction(server, entity);
        pthread_mutex_unlock(&game_state_mutex);
    }

    return NULL;
}

void server_main_loop(int server_socket_fd)
{
    double delta_time = TIME_PER_TURN;
    struct timeval last_update;
    gettimeofday(&last_update, NULL);

    INTERFACE *interface = interface_init();

    SERVER server;
    init(&server);

    server.game.server_pid = getpid();
    server.game.server_socket_fd = server_socket_fd;

    if (map_load(&server.map, MAP_FILE, &server.game.campsite))
    {
        // Could not load map.
        return;
    }

    pthread_t server_acceptance_thread;
    pthread_create(&server_acceptance_thread, NULL, acceptance_thread, (void *)&server);
    pthread_detach(server_acceptance_thread);

    while (server.game.running)
    {
        int key = getch();
        on_key_pressed(key, &server);
        update_timer(&delta_time, &last_update);

        if (delta_time < TIME_PER_TURN)
        {
            continue;
        }

        pthread_mutex_lock(&game_state_mutex);

        handle_game_state(&server);
        draw_server_interface(interface, &server);

        pthread_mutex_unlock(&game_state_mutex);

        delta_time -= TIME_PER_TURN;
        server.game.turns++;
    }

    pthread_cancel(server_acceptance_thread);
    destroy(&server);
    interface_destroy(interface);
}
