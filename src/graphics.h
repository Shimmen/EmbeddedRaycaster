#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdint.h>

//
// A framebuffer for rendering to the graphics display
//

#define FRAMEBUFFER_WIDTH  (128)
#define FRAMEBUFFER_HEIGHT (64)
#define FRAMEBUFFER_SIZE   (FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT)

typedef struct framebuffer
{
	uint8_t buffer[FRAMEBUFFER_SIZE];
} Framebuffer;

void framebuffer_clear(Framebuffer *framebuffer);

void framebuffer_set_pixel(Framebuffer *framebuffer, int x, int y, uint8_t value);
uint8_t framebuffer_get_pixel(Framebuffer *framebuffer, int x, int y);

void framebuffer_fill_ball(Framebuffer *framebuffer, int x0, int y0, uint8_t value);

void framebuffer_stroke_line_vert(Framebuffer *framebuffer, int x, int y0, int y1);
void framebuffer_stroke_line_hori(Framebuffer *framebuffer, int y, int x0, int x1);

#endif // GRAPHICS_H_