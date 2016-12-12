#ifndef MAZE_GENERATOR_H_
#define MAZE_GENERATOR_H_

#include <stdint.h>
#include "map.h"

#define MIN_MANHATTAN_DIST_FROM_START_TO_GOAL 4

#define RANDOM_SEED 0x12345

uint16_t random();
Map random_maze(int *tiles, int width, int height);


#endif // MAZE_GENERATOR_H_