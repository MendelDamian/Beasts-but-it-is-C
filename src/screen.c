#include "screen.h"

void screen_init(void)
{
    initscr();
    start_color();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(0);
    refresh();
    timeout(0);

    colors_init();
}

void draw_server_info(GAME *game)
{
    uint8_t y = 1;
    uint8_t x = 1;

    move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X);
    printw("Server's PID:  %d", game->server_pid);
    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    printw("Campsite X/Y: %02hhu/%02hhu", 23, 11);
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

void draw_server_interface(SERVER *server, GAME *game)
{
    if (server == NULL || game == NULL)
    {
        return;
    }

    // Draw map.
    map_draw(&server->map);

    // Draw sidebar's server info.
    draw_server_info(game);

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

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        PLAYER *player = &server->players[i];

        // Draw on sidebar.
        x = 15 + 10 * i;
        move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X + x);
        printw("Player %d", player->number);

        if (server->players[i].pid != 0)  // Or -1.
        {
            // Draw on map.
            player_draw(player);

            move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
            printw("%d", player->pid);
            move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
            printw("%s", player->type == HUMAN ? "HUMAN" : "CPU");
            move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
            printw("%02hhu/%02hhu", player->position.x, player->position.y);
            move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
            printw("%hhu", player->deaths);
            move(SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x);
            printw("%hu", player->carried_coins);
            move(SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x);
            printw("%hu", player->brought_coins);
        }
        else
        {
            move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
            printw("-         ");
            move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
            printw("-         ");
            move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
            printw("--/--     ");
            move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
            printw("-         ");
            move(SIDEBAR_OFFSET_Y + y + 7, SIDEBAR_OFFSET_X + x);
            printw("          ");
            move(SIDEBAR_OFFSET_Y + y + 8, SIDEBAR_OFFSET_X + x);
            printw("          ");
        }
    }

    // Draw legend.
    draw_legend();
}

void draw_client_interface(MAP_CHUNK *map_chunk, GAME *game, PLAYER *player)
{
    if (map_chunk == NULL || game == NULL || player == NULL)
    {
        return;
    }

    // Draw map.
    map_draw_chunk(map_chunk);

    // Draw player.
    player_draw(player);

    // Draw sidebar's server info.
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
    printw("%d", player->number);
    move(SIDEBAR_OFFSET_Y + y + 2, SIDEBAR_OFFSET_X + x);
    printw("%s", player->type == HUMAN ? "HUMAN" : "CPU");
    move(SIDEBAR_OFFSET_Y + y + 3, SIDEBAR_OFFSET_X + x);
    printw("%02hhu/%02hhu", player->position.x, player->position.y);
    move(SIDEBAR_OFFSET_Y + y + 4, SIDEBAR_OFFSET_X + x);
    printw("%hhu", player->deaths);

    // Draw coin info.
    y = 11;
    x = 0;

    move(SIDEBAR_OFFSET_Y + y, SIDEBAR_OFFSET_X + x);
    printw("Coins found:   %d", player->carried_coins + player->brought_coins);
    move(SIDEBAR_OFFSET_Y + y + 1, SIDEBAR_OFFSET_X + x);
    printw("Coins brought: %d", player->brought_coins);

    // Draw legend.
    draw_legend();
}
