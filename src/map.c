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

            map->tiles[y][x] = line[x];
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

void map_draw(MAP *map, uint8_t x_offset, uint8_t y_offset)
{
    if (map == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < map->height; ++i)
    {
        for (uint8_t j = 0; j < map->width; ++j)
        {
            uint8_t x = x_offset + j;
            uint8_t y = y_offset + i;

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
                case TILE_SINGLE_COIN:
                    color = COLOR_PAIR(PAIR_COIN);
                    break;
            }

            attron(color);
            mvaddch(y, x, map->tiles[i][j]);
            attroff(color);
        }
    }
}

char tile2char(TILE tile)
{
    switch (tile)
    {
        case TILE_EMPTY:
            return ' ';
        case TILE_WALL:
            return 'W';
        case TILE_BUSH:
            return 'x';
        case TILE_CAMPSITE:
            return 'A';
        case TILE_SINGLE_COIN:
            return 'c';
        case TILE_TREASURE:
            return 't';
        case TILE_LARGE_TREASURE:
            return 'T';
        default:
            return tile2char(TILE_EMPTY);
    }
}

TILE char2tile(char c)
{
    switch (c)
    {
        case ' ':
            return TILE_EMPTY;
        case 'W':
            return TILE_WALL;
        case 'x':
            return TILE_BUSH;
        case 'A':
            return TILE_CAMPSITE;
        case 'c':
            return TILE_SINGLE_COIN;
        case 't':
            return TILE_TREASURE;
        case 'T':
            return TILE_LARGE_TREASURE;
        default:
            return char2tile(' ');
    }
}
