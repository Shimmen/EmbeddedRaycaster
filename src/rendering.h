#ifndef RENDERING_H_
#define RENDERING_H_

#include <stdbool.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

void screen_setup();
void screen_clear();
void screen_push_rendered();

void draw_column(int x, int draw_start, int draw_end, bool is_corner);

#endif // RENDERING_H_
