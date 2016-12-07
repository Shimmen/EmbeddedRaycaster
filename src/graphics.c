#include "graphics.h"

void framebuffer_clear(Framebuffer *framebuffer)
{
	for (int i = 0; i < FRAMEBUFFER_SIZE; ++i) {
		//framebuffer->buffer[i] = 0x55;
		framebuffer->buffer[i] = 0x00;
	}
}

void framebuffer_set_pixel(Framebuffer *framebuffer, int x, int y, uint8_t value)
{
	if (x >= 0 && x < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) {
		framebuffer->buffer[x + y * FRAMEBUFFER_WIDTH] = value;
	}
}

uint8_t framebuffer_get_pixel(Framebuffer *framebuffer, int x, int y)
{
	if (x < 0 || x > FRAMEBUFFER_WIDTH || y < 0 || y > FRAMEBUFFER_HEIGHT) {
		return 0x00;
	}
	
	return framebuffer->buffer[x + y * FRAMEBUFFER_WIDTH];
}

void framebuffer_fill_ball(Framebuffer *framebuffer, int x0, int y0, uint8_t value)
{
	const int ball_radius = 4;
	
	const int x_min = x0 - ball_radius;
	const int x_max = x0 + ball_radius;
	const int y_min = y0 - ball_radius;
	const int y_max = y0 + ball_radius;
	
	for (int y = y_min; y <= y_max; y++) {
		for (int x = x_min; x <= x_max; x++) {
			framebuffer_set_pixel(framebuffer, x, y, value);
		}
	}
}

void framebuffer_stroke_line_vert(Framebuffer *framebuffer, int x, int y0, int y1)
{
	if (y0 > y1||x < 0||x >= 128) {
		return;
	}
	
	for (int y = y0; y <= y1; y++) {
		framebuffer_set_pixel(framebuffer, x, y, 1);
	}
}

void framebuffer_stroke_line_hori(Framebuffer *framebuffer, int y, int x0, int x1)
{
	if (x0 > x1||y < 0||y >= 64) {
		return;
	}
	
	for (int x = x0; x <= x1; x++) {
		framebuffer_set_pixel(framebuffer, x, y, 1);
	}
}
