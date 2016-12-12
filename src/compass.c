#include "compass.h"

char compass_string_close[] = ". . . . | . . . X . . . . | . . . . |";
char compass_string_far[] =   ". . . . | . . . x . . . . | . . . . |";
int compass_length = 37;

float angle_between_player_and_goal(Vec2 player_pos, Vec2 goal_pos) {
    // TODO: IMPLEMENT!
    return 0.5f;
}

void cycle_string(char source[], char target[], int steps) {
    for (int i = 0; i < compass_length; i++) {
        target[(i+steps) % compass_length] = source[i];
    }
}

char *get_compass_string(Vec2 player_pos, Vec2 goal_pos) {
    char compass_string[compass_length];
    int steps = angle_between_player_and_goal(player_pos, goal_pos) * compass_length / 360.0f;
    cycle_string(compass_string_close, compass_string, steps);
    return compass_string;
}