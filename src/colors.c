#include "colors.h"

#include <ncurses.h>

void colors_init(void)
{
    start_color();
    init_pair(PAIR_DEFAULT, -1, -1);
    init_pair(PAIR_PLAYER, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(PAIR_BEAST, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COIN, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PAIR_CAMPSITE, COLOR_RED, COLOR_GREEN);
    init_pair(PAIR_BUSH, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR_WALL, COLOR_WHITE, COLOR_WHITE);
    init_pair(PAIR_EMPTY, COLOR_BLACK, COLOR_BLACK);
}
