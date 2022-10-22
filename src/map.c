#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map.h"

void map_init(MAP *map)
{
    memset(map, 0, sizeof(MAP));
}

int map_load(MAP *map, const char *filename, COORDS *campsite_position)
{
    if (map == NULL || filename == NULL || campsite_position == NULL)
    {
        return -1;
    }

    map_init(map);

    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        return 2;
    }

    char line[MAX_MAP_WIDTH + 1];
    int y = 0;

    while (fgets(line, sizeof(line), f) != NULL)
    {
        if (y >= MAX_MAP_HEIGHT)
        {
            fclose(f);
            return 3;
        }

        int x = 0;
        while (line[x] != '\0' && line[x] != '\r' && line[x] != '\n')
        {
            if (x >= MAX_MAP_WIDTH)
            {
                fclose(f);
                return 4;
            }

            map->tiles[y][x] = line[x];
            if (map->tiles[y][x] == TILE_CAMPSITE)
            {
                campsite_position->x = x;
                campsite_position->y = y;
            }
            x++;
        }

        if (x == 0)
        {
            fclose(f);
            return 6;
        }

        if (map->width == 0)
        {
            map->width = x;
        }
        else if (map->width != x)
        {
            fclose(f);
            return 7;
        }

        y++;
    }

    map->height = y;
    return 0;
}

void map_get_chunk(MAP *map, MAP_CHUNK *chunk, COORDS position)
{
    if (map == NULL || chunk == NULL)
    {
        return;
    }

    chunk->position = position;
    chunk->width = MAP_CHUNK_WIDTH;
    chunk->height = MAP_CHUNK_HEIGHT;

    for (uint8_t i = 0; i < chunk->height; ++i)
    {
        for (uint8_t j = 0; j < chunk->width; ++j)
        {
            uint8_t x = chunk->position.x + j - (chunk->width / 2);
            uint8_t y = chunk->position.y + i - (chunk->height / 2);

            if (x >= map->width || y >= map->height)
            {
                chunk->tiles[i][j] = TILE_EMPTY;
            }
            else
            {
                chunk->tiles[i][j] = map->tiles[y][x];
            }
        }
    }
}

COORDS map_chunk_get_campsite(MAP_CHUNK *chunk)
{
    COORDS coords = {0, 0};
    if (chunk == NULL)
    {
        return coords;
    }

    for (uint8_t i = 0; i < chunk->height; ++i)
    {
        for (uint8_t j = 0; j < chunk->width; ++j)
        {
            if (chunk->tiles[i][j] == TILE_CAMPSITE)
            {
                coords.x = chunk->position.x + j - (chunk->width / 2);
                coords.y = chunk->position.y + i - (chunk->height / 2);
                return coords;
            }
        }
    }

    return coords;
}
