
#include "maze_generator.h"
#include "stdbool.h"


#define SOUTH width
#define WEST (-1)
#define NORTH (-width)
#define EAST 1
typedef int Direction;

#define ABS(x) (x < 0 ? -x : x)

//
// Pseudo-random number generator
//
uint16_t lfsr = RANDOM_SEED;
uint16_t bit;
uint16_t random() {
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr = (lfsr >> 1) | (bit << 15);
}

//
// Maze generator
//

// Returns a random maze by modifying the specified tile array and adding a reference to it in a new Map
Map random_maze(int *tiles, int width, int height) {
    
    // Round width and height up to nearest odd numbers
    if (width%2 == 0)
        width++;
    if (height%2 == 0)
        height++;
    
    // Begin with walls everywhere
    for (int i = 0; i < width*height; i++) {
        tiles[i] = WALL;
    }
    
    bool is_within_bounds(Direction direction, int pos, int neighbor) {
        return !(direction == WEST && pos % width == 1) && !(direction == EAST && pos % width == width-2) && neighbor >= 0 && neighbor < width*height;
    }
    
    // Generates an array of the four directions in random order
    Direction * randomly_ordered_directions() {
        Direction possible[4] = { SOUTH, WEST, NORTH, EAST };
        static Direction ordering[4];
        for (int i = 0; i < 4; i++) {
            int next_direction_index = random() % (4-i); // The index of the next direction to add, skipping already added directions in indexing
    
            // Find the direction with the given next_direction_index
            int index = 0;
            for (int j = 0; j < 4; j++) {
                for (int ordering_index = 0; ordering_index < i; ordering_index++) { // Iterate through already added directions
                    if (ordering[ordering_index] == possible[j]) { // If direction already added
                        index--; // Skip already added direction in indexing
                        break;
                    }
                }
                if (index == next_direction_index) {
                    ordering[i] = possible[j]; // Direction found, add it to the ordering
                    break;
                }
                index++;
            }
        }
        return ordering;
    }
    
    void recursive_backtracking(int pos) {
        Direction *dirs = randomly_ordered_directions();
        for (int i = 0; i < 4; i++) {
            Direction direction = dirs[i];
            int wall = pos + direction;
            int neighbor = pos + direction * 2;
            // Check if the position is within map bounds
            if (is_within_bounds(direction, pos, neighbor)) {
                // Check if the neighbor is unvisited
                if (tiles[neighbor] == WALL) {
                    // Clear neighbor and wall, continue from neighbor
                    tiles[wall] = EMPTY;
                    tiles[neighbor] = EMPTY;
                    recursive_backtracking(neighbor);
                }
            }
        }
    }
    
    int manhattan_dist(pos1, pos2) {
        return ABS((pos1 % width) - (pos2 % width)) + ABS((pos1 / width) - (pos2 / width));
    }
    
    int randomize_goal_pos() {
        int x = 2 + 2*(random() % ((width-5)/2));
        int y = 2 + 2*(random() % ((height-5)/2));
        return x + y * width;
    }
    
    int randomize_start_pos(int goal_pos) {
        int x, y, pos;
        do {
            x = 1 + random() % (width-2);
            y = 1 + random() % (height-2);
            pos = x + y * width;
        } while (tiles[pos] == WALL || manhattan_dist(pos, goal_pos) < MIN_MANHATTAN_DIST_FROM_START_TO_GOAL);
        return pos;
    }
    
    // Creates the goal room with a black box in the middle
    void create_goal_room(int goal_pos) {
        tiles[goal_pos] = GOAL;
        tiles[goal_pos + SOUTH] = EMPTY;
        tiles[goal_pos + NORTH] = EMPTY;
        tiles[goal_pos + WEST] = EMPTY;
        tiles[goal_pos + EAST] = EMPTY;
        tiles[goal_pos + SOUTH + WEST] = EMPTY;
        tiles[goal_pos + NORTH + WEST] = EMPTY;
        tiles[goal_pos + SOUTH + EAST] = EMPTY;
        tiles[goal_pos + NORTH + EAST] = EMPTY;
    }
    
    // Randomizes a position adjacent to the goal from where the maze generation can start
    int randomize_goal_entryway(int goal_pos) {
        bool x_aligned = (goal_pos % width) % 2 == 1;
        bool y_aligned = (goal_pos / width) % 2 == 1;
        bool south_free = (goal_pos / width) < height-2;
        bool west_free = (goal_pos % width) >= 2;
        bool north_free = (goal_pos / width) >= 2;
        bool east_free = (goal_pos % width) < width-2;
        int possible_entry_points[8];
        int i = 0;
        if ((south_free || west_free) && !x_aligned && !y_aligned) {possible_entry_points[i] = goal_pos + SOUTH + WEST; i++;}
        if ((north_free || west_free) && !x_aligned && !y_aligned) {possible_entry_points[i] = goal_pos + NORTH + WEST; i++;}
        if ((south_free || east_free) && !x_aligned && !y_aligned) {possible_entry_points[i] = goal_pos + SOUTH + EAST; i++;}
        if ((north_free || east_free) && !x_aligned && !y_aligned) {possible_entry_points[i] = goal_pos + NORTH + EAST; i++;}
        
        // Select randomly from the possible entry points
        int selected_entry_index = random() % i;
        
        // Select randomly from the entry point's possible entryways
        int entry_point = possible_entry_points[selected_entry_index];
        Direction *dirs = randomly_ordered_directions();
        for (int j = 0; j < 4; j++) {
            Direction direction = dirs[i];
            int wall = entry_point + direction;
            int neighbor = entry_point + direction * 2;
            if (is_within_bounds(direction, entry_point, neighbor)) {
                tiles[wall] = EMPTY;
                tiles[neighbor] = EMPTY;
                return neighbor;
            }
        }
    }
    
    int goal_pos = randomize_goal_pos();
    int goal_entry = randomize_goal_entryway(goal_pos);
    create_goal_room(goal_pos);
    recursive_backtracking(goal_entry);
    int start = randomize_start_pos(goal_pos);
    
    const Map map = {
        width,
        height,
        &tiles[0],
        {start%width, start/width},
        {-1, 0},
        {0.0f, 0.66f}
    };
    
    return map;
}