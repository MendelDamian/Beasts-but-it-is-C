#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <ncurses.h>

#include "conf.h"
#include "colors.h"
#include "coordinate.h"

#define MAX_MAP_WIDTH  128
#define MAX_MAP_HEIGHT 128

typedef enum tile_t
{
    TILE_EMPTY = ' ',
    TILE_WALL = 'W',
    TILE_BUSH = '#',
    TILE_CAMPSITE = 'A',
    TILE_SINGLE_COIN = 'c',
    TILE_TREASURE = 't',
    TILE_LARGE_TREASURE = 'T',
} TILE;

typedef struct map_t
{
    uint8_t width;
    uint8_t height;
    char tiles[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];
} MAP;

typedef struct map_chunk_t
{
    COORDS position;  // Center of the chunk.
    uint8_t width;  // Width of the chunk.
    uint8_t height;  // Height of the chunk.
    char tiles[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];
} MAP_CHUNK;

void map_init(MAP *map);
int map_load(MAP *map, const char *filename);
void map_draw(MAP *map, uint8_t x_offset, uint8_t y_offset);
void map_draw_chunk(MAP_CHUNK *chunk, uint8_t x_offset, uint8_t y_offset);
void map_get_chunk(MAP *map, MAP_CHUNK *chunk, COORDS position);

#endif //MAP_H
