#include <stdio.h>
#include <string.h>
#include "map.h"

void map_init(MAP *map)
{
    map->width = 0;
    map->height = 0;
    memset(map->tiles, 0, sizeof(map->tiles));
}

int map_load(MAP *map, const char *filename)
{
    if (map == NULL || filename == NULL)
    {
        return 1;
    }

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

            switch (line[x])
            {
            case ' ':
                map->tiles[y][x] = TILE_EMPTY;
                break;
            case 'X':
                map->tiles[y][x] = TILE_WALL;
            case '#':
                map->tiles[y][x] = TILE_BUSH;
                break;
            case 'A':
                map->tiles[y][x] = TILE_CAMPSITE;
                break;
            case 'c':
                map->tiles[y][x] = TILE_SINGLE_COIN;
                break;
            case 't':
                map->tiles[y][x] = TILE_TREASURE;
                break;
            case 'T':
                map->tiles[y][x] = TILE_LARGE_TREASURE;
                break;
            default:
                fclose(f);
                return 5;
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

    return 0;
}

// Extract drawing to window module.
void map_draw(MAP *map, uint8_t x_offset, uint8_t y_offset)
{

}
