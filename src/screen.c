#include "screen.h"

void screen_init(void)
{
    initscr();
    start_color();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(0);
    timeout(50);

    colors_init();
}

void draw_server_info(GAME *game)
{
    uint8_t y = 1;
    uint8_t x = 1;

    move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    printw("Server's PID:  %d", game->server_pid);
    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    if (game->campsite.x == 0 && game->campsite.y == 0)
    {
        printw("Campsite X/Y: unknown", game->campsite.x, game->campsite.y);
    }
    else
    {
        printw("Campsite X/Y: %02hhu/%02hhu  ", game->campsite.x, game->campsite.y);
    }
    move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    printw("Round number: %u", game->turns);
}

void draw_legend()
{
    uint8_t y = 16;
    uint8_t x = 1;

    move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    printw("Legend:");

    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_PLAYER));
    printw("1234");
    attroff(COLOR_PAIR(PAIR_PLAYER));
    printw(" - players");

    move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_WALL));
    printw("%c", TILE_WALL);
    attroff(COLOR_PAIR(PAIR_WALL));
    printw("    - wall");

    move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_BUSH));
    printw("%c", TILE_BUSH);
    attroff(COLOR_PAIR(PAIR_BUSH));
    printw("    - bush");

    move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_BEAST));
    printw("%c", TILE_BEAST);
    attroff(COLOR_PAIR(PAIR_BEAST));
    printw("    - wild beast");

    move(SIDEBAR_OFFSET_Y + y + 5, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_COIN));
    printw("%c", TILE_COIN);
    attroff(COLOR_PAIR(PAIR_COIN));
    printw("    - coin");

    move(SIDEBAR_OFFSET_Y + y + 6, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_COIN));
    printw("%c", TILE_TREASURE);
    attroff(COLOR_PAIR(PAIR_COIN));
    printw("    - treasure");

    move(SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_COIN));
    printw("%c", TILE_LARGE_TREASURE);
    attroff(COLOR_PAIR(PAIR_COIN));
    printw("    - large treasure");

    move(SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_COIN));
    printw("%c", TILE_DROPPED_TREASURE);
    attroff(COLOR_PAIR(PAIR_COIN));
    printw("    - dropped treasure");

    move(SIDEBAR_OFFSET_Y + y + 9, SIDEBAR_OFFSET_X + x);
    attron(COLOR_PAIR(PAIR_CAMPSITE));
    printw("%c", TILE_CAMPSITE);
    attroff(COLOR_PAIR(PAIR_CAMPSITE));
    printw("    - campsite");
}

void draw_server_interface(SERVER *server, bool full_draw)
{
    if (server == NULL)
    {
        return;
    }

    if (full_draw)
    {
        clear();

        // Draw legend.
        draw_legend();
    }

    // Draw server info.
    draw_server_info(&server->game);

    // Draw map.
    map_draw(&server->map);

    // Draw sidebar's players section.
    uint8_t y = 5;
    uint8_t x = 1;

    move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    printw("Parameter:");
    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    printw("PID");
    move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    printw("Type");
    move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    printw("Curr XX/YY");
    move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    printw("Deaths");

    move(SIDEBAR_OFFSET_Y + y + 6, SIDEBAR_OFFSET_X + x);
    printw("Coins");
    move(SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x + 5);
    printw("carried");
    move(SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x + 5);
    printw("brought");

    NODE *node = server->entities->head;
    while (node)
    {
        ENTITY *entity = (ENTITY *)node->item;
        if (entity->type != ENTITY_TYPE_BEAST)
        {
            // Draw on sidebar.
            x = 15 + 10 * (entity->number - 1);
            move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X + x);
            printw("Player %d", entity->number);

            // Draw on map.
            draw_entity(entity);

            move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
            printw("%d", entity->pid);
            move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
            printw("%s", get_entity_type_name(entity->type));
            move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
            printw("%02hhu/%02hhu", entity->position.x, entity->position.y);
            move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
            printw("%hhu", entity->deaths);
            move(SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x);
            printw("%hu", entity->carried_coins);
            move(SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x);
            printw("%hu", entity->brought_coins);
        }

        node = node->next;
    }
}

void draw_client_interface(MAP_CHUNK *map_chunk, GAME *game, ENTITY *entity, bool full_draw)
{
    if (map_chunk == NULL || game == NULL || entity == NULL)
    {
        return;
    }

    // Draw map.
    map_draw_chunk(map_chunk);

    if (full_draw)
    {
        // Draw legend.
        draw_legend();
    }

    // Draw server info.
    draw_server_info(game);

    // Draw sidebar's players section.
    uint8_t y = 5;
    uint8_t x = 1;

    move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    printw("Player:");
    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    printw("Number:");
    move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    printw("Type:");
    move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    printw("Curr XX/YY:");
    move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    printw("Deaths:");

    // Draw on sidebar.
    x = 15;
    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    printw("%d", entity->number);
    move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    printw("%s", get_entity_type_name(entity->type));
    move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    printw("%02hhu/%02hhu", entity->position.x, entity->position.y);
    move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    printw("%hhu", entity->deaths);

    // Draw coin info.
    y = 11;
    x = 0;

    move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X + x);
    printw("Coins found:   %d", entity->carried_coins + entity->brought_coins);
    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    printw("Coins brought: %d", entity->brought_coins);

}
