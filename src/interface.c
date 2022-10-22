#include "interface.h"

INTERFACE *interface_init(void)
{
    initscr();
    start_color();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(0);
    timeout(50);

    INTERFACE *interface = calloc(1, sizeof(INTERFACE));
    interface->height = INTERFACE_HEIGHT;
    interface->width = INTERFACE_WIDTH;
    interface->win = newwin(interface->height, interface->width, INTERFACE_OFFSET_Y, INTERFACE_OFFSET_X);
    refresh();

    colors_init();

    return interface;
}

void interface_destroy(INTERFACE *interface)
{
    if (interface == NULL)
    {
        return;
    }

    delwin(interface->win);
    free(interface);
    endwin();
}

void draw_server_info(INTERFACE *interface, GAME *game)
{
    uint8_t y = 0;
    uint8_t x = 1;

    wmove(interface->win, y, SIDEBAR_OFFSET_X);
    wprintw(interface->win, "Server's PID:  %d", game->server_pid);
    wmove(interface->win, y + 1, SIDEBAR_OFFSET_X + x);
    if (game->campsite.x == 0 && game->campsite.y == 0)
    {
        wprintw(interface->win, "Campsite X/Y: unknown");
    }
    else
    {
        wprintw(interface->win, "Campsite X/Y: %02hhu/%02hhu  ", game->campsite.x, game->campsite.y);
    }
    wmove(interface->win, y + 2, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Round number: %u", game->turns);
}

void draw_legend(INTERFACE *interface)
{
    uint8_t y = 14;
    uint8_t x = 1;

    wmove(interface->win, SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    wprintw(interface->win, "Legend:");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_PLAYER));
    wprintw(interface->win, "1234");
    wattroff(interface->win, COLOR_PAIR(PAIR_PLAYER));
    wprintw(interface->win, " - players");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_WALL));
    wprintw(interface->win, "%c", TILE_WALL);
    wattroff(interface->win, COLOR_PAIR(PAIR_WALL));
    wprintw(interface->win, " - wall");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_CAMPSITE));
    wprintw(interface->win, "%c", TILE_CAMPSITE);
    wattroff(interface->win, COLOR_PAIR(PAIR_CAMPSITE));
    wprintw(interface->win, " - campsite");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_BUSH));
    wprintw(interface->win, "%c", TILE_BUSH);
    wattroff(interface->win, COLOR_PAIR(PAIR_BUSH));
    wprintw(interface->win, " - bush");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 5, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_BEAST));
    wprintw(interface->win, "%c", TILE_BEAST);
    wattroff(interface->win, COLOR_PAIR(PAIR_BEAST));
    wprintw(interface->win, " - wild beast");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 6, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, "%c", TILE_COIN);
    wattroff(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, " - coin");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, "%c", TILE_TREASURE);
    wattroff(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, " - treasure");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, "%c", TILE_LARGE_TREASURE);
    wattroff(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, " - large treasure");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 9, SIDEBAR_OFFSET_X + x);
    wattron(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, "%c", TILE_DROPPED_TREASURE);
    wattroff(interface->win, COLOR_PAIR(PAIR_COIN));
    wprintw(interface->win, " - dropped treasure");
}

void draw_server_interface(INTERFACE *interface, SERVER *server)
{
    if (interface == NULL || server == NULL)
    {
        return;
    }

    // Draw legend.
    draw_legend(interface);
//
    // Draw server info.
    draw_server_info(interface, &server->game);

    // Draw map.
    draw_map(interface, &server->map);

    // Draw sidebar's players section.
    uint8_t y = 3;
    uint8_t x = 1;

    wmove(interface->win, SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    wprintw(interface->win, "Parameter:");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "PID");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Type");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Curr XX/YY");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Deaths");

    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 6, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Coins");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x + 5);
    wprintw(interface->win, "carried");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x + 5);
    wprintw(interface->win, "brought");

    // Clear players section.
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        x = 15 + 10 * i;
        mvwprintw(interface->win, SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X + x, "          ");
        mvwprintw(interface->win, SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x, "          ");
        mvwprintw(interface->win, SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x, "          ");
        mvwprintw(interface->win, SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x, "          ");
        mvwprintw(interface->win, SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x, "          ");
        mvwprintw(interface->win, SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x, "          ");
        mvwprintw(interface->win, SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x, "          ");
    }

    NODE *node = server->entities->head;
    while (node)
    {
        ENTITY *entity = (ENTITY *)node->item;
        if (entity->type != ENTITY_TYPE_BEAST)
        {
            // Draw on sidebar.
            x = 15 + 10 * (entity->number - 1);
            wmove(interface->win, SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X + x);
            wprintw(interface->win, "Player %d", entity->number);

            // Draw on map.
            draw_entity(interface, entity);

            wmove(interface->win, SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
            wprintw(interface->win, "%d", entity->pid);
            wmove(interface->win, SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
            wprintw(interface->win, "%s", get_entity_type_name(entity->type));
            wmove(interface->win, SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
            wprintw(interface->win, "%02hhu/%02hhu", entity->position.x, entity->position.y);
            wmove(interface->win, SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
            wprintw(interface->win, "%hhu", entity->deaths);
            wmove(interface->win, SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x);
            wprintw(interface->win, "%hu", entity->carried_coins);
            wmove(interface->win, SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x);
            wprintw(interface->win, "%hu", entity->brought_coins);
        }

        node = node->next;
    }

    wrefresh(interface->win);
}

void draw_client_interface(INTERFACE *interface, MAP_CHUNK *map_chunk, GAME *game, ENTITY *entity)
{
    if (interface == NULL || map_chunk == NULL || game == NULL || entity == NULL)
    {
        return;
    }

    // Clear space for a map.
    for (int i = 0; i < INTERFACE_HEIGHT; ++i)
    {
        for (int j = 0; j < SIDEBAR_OFFSET_X; ++j)
        {
            mvwaddch(interface->win, i, j, TILE_EMPTY);
        }
    }

    // Draw map.
    draw_map_chunk(interface, map_chunk);

    // Draw legend.
    draw_legend(interface);

    // Draw server info.
    draw_server_info(interface, game);

    // Draw sidebar's players section.
    uint8_t y = 4;
    uint8_t x = 1;

    wmove(interface->win, SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    wprintw(interface->win, "Player:");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Number:");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Type:");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Curr XX/YY:");
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Deaths:");

    // Draw on sidebar.
    x = 15;
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "%d", entity->number);
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "%s", get_entity_type_name(entity->type));
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "%02hhu/%02hhu", entity->position.x, entity->position.y);
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "%hhu", entity->deaths);

    // Draw coin info.
    y = 10;
    x = 0;

    wmove(interface->win, SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Coins found:   %d", entity->carried_coins + entity->brought_coins);
    wmove(interface->win, SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    wprintw(interface->win, "Coins brought: %d", entity->brought_coins);

    wrefresh(interface->win);
}

void draw_entity(INTERFACE *interface, ENTITY *entity)
{
    if (interface == NULL || entity == NULL)
    {
        return;
    }

    switch (entity->type)
    {
        case ENTITY_TYPE_BOT:
        case ENTITY_TYPE_PLAYER:
            wattron(interface->win, COLOR_PAIR(PAIR_PLAYER));
            mvwaddch(interface->win, entity->position.y, entity->position.x, entity->number + '0');
            wattroff(interface->win, COLOR_PAIR(PAIR_PLAYER));
            break;

        case ENTITY_TYPE_BEAST:
            wattron(interface->win, COLOR_PAIR(PAIR_BEAST));
            mvwaddch(interface->win, entity->position.y, entity->position.x, TILE_BEAST);
            wattroff(interface->win, COLOR_PAIR(PAIR_BEAST));
            break;

        default:
            break;
    }
}

void draw_map(INTERFACE *interface, MAP *map)
{
    if (map == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < map->height; ++i)
    {
        for (uint8_t j = 0; j < map->width; ++j)
        {
            enum color_pair_t color = PAIR_DEFAULT;

            switch (map->tiles[i][j])
            {
                default:
                case TILE_EMPTY:
                    color = COLOR_PAIR(PAIR_EMPTY);
                    break;

                case TILE_WALL:
                    color = COLOR_PAIR(PAIR_WALL);
                    break;

                case TILE_BUSH:
                    color = COLOR_PAIR(PAIR_BUSH);
                    break;

                case TILE_CAMPSITE:
                    color = COLOR_PAIR(PAIR_CAMPSITE);
                    break;

                case TILE_TREASURE:
                case TILE_LARGE_TREASURE:
                case TILE_COIN:
                    color = COLOR_PAIR(PAIR_COIN);
                    break;
            }

            wattron(interface->win, color);
            mvwaddch(interface->win, i, j, map->tiles[i][j]);
            wattroff(interface->win, color);
        }
    }
}

void draw_map_chunk(INTERFACE *interface, MAP_CHUNK *chunk)
{
    if (chunk == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < chunk->height; ++i)
    {
        for (uint8_t j = 0; j < chunk->width; ++j)
        {
            uint8_t x = chunk->position.x + j - chunk->width / 2;
            uint8_t y = chunk->position.y + i - chunk->width / 2;
            enum color_pair_t color = PAIR_DEFAULT;

            switch (chunk->tiles[i][j])
            {
                case '1':
                case '2':
                case '3':
                case '4':
                    color = COLOR_PAIR(PAIR_PLAYER);
                    break;

                default:
                case TILE_EMPTY:
                    color = COLOR_PAIR(PAIR_EMPTY);
                    break;

                case TILE_WALL:
                    color = COLOR_PAIR(PAIR_WALL);
                    break;

                case TILE_BUSH:
                    color = COLOR_PAIR(PAIR_BUSH);
                    break;

                case TILE_CAMPSITE:
                    color = COLOR_PAIR(PAIR_CAMPSITE);
                    break;

                case TILE_TREASURE:
                case TILE_LARGE_TREASURE:
                case TILE_COIN:
                    color = COLOR_PAIR(PAIR_COIN);
                    break;
            }

            wattron(interface->win, color);
            mvwaddch(interface->win, y, x, chunk->tiles[i][j]);
            wattroff(interface->win, color);
        }
    }
}
