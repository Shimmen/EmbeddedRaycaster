#ifndef GRAPHICS_DISPLAY_H_
#define GRAPHICS_DISPLAY_H_

#include <stdint.h>

#include "gpio.h"
#include "graphics.h"

#define GRAPHICS_DISPLAY_WIDTH  (128)
#define GRAPHICS_DISPLAY_HEIGHT (64)

typedef struct graphics_display {
	GPIO *gpio;
} GraphicsDisplay;

//
// Create a graphics display at the given GPIO
//
GraphicsDisplay graphics_display_create(GPIO *gpio);

//
// Clear the display to blank (all pixels off / not black)
//
void graphics_display_clear(GraphicsDisplay *display);

//
// Copy and show the supplied framebuffer to the display
//
void graphics_display_push_framebuffer(GraphicsDisplay *display, Framebuffer *framebuffer);

#endif // GRAPHICS_DISPLAY_H_