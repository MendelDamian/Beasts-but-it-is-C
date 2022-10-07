#ifndef COLORS_H
#define COLORS_H

enum color_pair_t
{
    PAIR_DEFAULT,
    PAIR_PLAYER,
    PAIR_COIN,
    PAIR_CAMPSITE,
    PAIR_BUSH,
    PAIR_WALL,
    PAIR_EMPTY
};

void colors_init(void);

#endif //COLORS_H
