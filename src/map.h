#ifndef MAP_H_
#define MAP_H_

#include "math.h"

#define EMPTY 0
#define WALL 1
#define GOAL 2

typedef struct map {
    int width;
    int height;
    int *tiles;

    Vec2 start_pos;
    Vec2 start_dir;
    Vec2 start_camera_plane;
} Map;

int get_tile(const Map *map, int x, int y);

extern const Map default_map;
extern const Map line_map;

void debug_print_map(Map *map);

#endif // MAP_H_