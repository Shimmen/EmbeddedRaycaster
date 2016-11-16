#ifndef MAP_H_
#define MAP_H_

typedef struct map {
    int width;
    int height;
    int *tiles;
} Map;

int get_tile(const Map *map, int x, int y);

extern const Map default_map;

#endif // MAP_H_