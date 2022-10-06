#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#define MAX_MAP_WIDTH  128
#define MAX_MAP_HEIGHT 128

typedef enum tile_t
{
    TILE_EMPTY = 1,
    TILE_WALL = 2,
    TILE_BUSH = 3,
    TILE_CAMPSITE = 4,
    TILE_SINGLE_COIN = 5,
    TILE_TREASURE = 6,
    TILE_LARGE_TREASURE = 7,
} TILE;

typedef struct map_t
{
    uint8_t width;
    uint8_t height;
    uint8_t tiles[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];
} MAP;

void map_init(MAP *map);
int map_load(MAP *map, const char *filename);
void map_draw(MAP *map, uint8_t x_offset, uint8_t y_offset);

#endif //MAP_H
