#include "rendering.h"

#if !EMBEDDED_MODE

#include <SDL2/SDL.h>

static SDL_Window *window;
static SDL_Renderer *renderer;

void screen_setup()
{
    static const int screen_scale = 4;
    window = SDL_CreateWindow(
            "EmbeddedRaycast", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH * screen_scale, SCREEN_HEIGHT * screen_scale, 0
    );
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetScale(renderer, screen_scale, screen_scale);
}

void screen_clear()
{
    SDL_SetRenderDrawColor(renderer, 108, 160, 31, 255);
    SDL_RenderClear(renderer);
}

void screen_push_rendered()
{
    SDL_RenderPresent(renderer);

    // Add some delay to slow the loop down just a bit
    SDL_Delay(1);
}

void draw_column(int x, int draw_start, int draw_end, bool fill_column)
{
    if (draw_start > draw_end)
    {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 20, 44, 13, 255);

    if (draw_start >= 0 && draw_start < SCREEN_HEIGHT) SDL_RenderDrawPoint(renderer, x, draw_start);
    if (draw_end >= 0 && draw_end < SCREEN_HEIGHT) SDL_RenderDrawPoint(renderer, x, draw_end);

    for (int y = draw_start; y < draw_end; y += 1)
    {
        if (fill_column)
        {
            SDL_RenderDrawPoint(renderer, x, y);
        }
        else
        {
            if (y < draw_start + ((draw_end - draw_start) / 7.0f))
            {
                if (y >= 0 && y < SCREEN_HEIGHT)
                {
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }

            if (y > draw_start + ((6 * (draw_end - draw_start)) / 7.0f))
            {
                if (y >= 0 && y < SCREEN_HEIGHT)
                {
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }
}

#else // if EMBEDDED_MODE

#include "gpio.h"
#include "graphics_display.h"
#include "graphics.h"
#include "timer.h"

GraphicsDisplay display;
Framebuffer framebuffer;

void screen_setup()
{
    // Initialize the timer (only required for the display)
    timer_init();

    GPIO *gpio_e = gpio_create(GPIO_E);
    display = graphics_display_create(gpio_e);
    graphics_display_clear(&display);
    framebuffer_clear(&framebuffer);
}

void screen_clear()
{
    framebuffer_clear(&framebuffer);
}

void screen_push_rendered()
{
    graphics_display_push_framebuffer(&display, &framebuffer);
}

void draw_column(int x, int draw_start, int draw_end, bool fill_column)
{
    if (draw_start > draw_end)
    {
        return;
    }

    if (draw_start >= 0 && draw_start < SCREEN_HEIGHT) framebuffer_set_pixel(&framebuffer, x, draw_start, 1);
    if (draw_end >= 0 && draw_end < SCREEN_HEIGHT) framebuffer_set_pixel(&framebuffer, x, draw_end, 1);

    for (int y = draw_start; y < draw_end; y += 1)
    {
        if (fill_column)
        {
            framebuffer_set_pixel(&framebuffer, x, y, 1);
        }
        else
        {
            if (y < draw_start + ((draw_end - draw_start) / 7))
            {
                if (y >= 0 && y < SCREEN_HEIGHT)
                {
                    framebuffer_set_pixel(&framebuffer, x, y, 1);
                }
            }

            if (y > draw_start + ((6 * (draw_end - draw_start)) / 7.0f))
            {
                if (y >= 0 && y < SCREEN_HEIGHT)
                {
                    framebuffer_set_pixel(&framebuffer, x, y, 1);
                }
            }
        }
    }
}

#endif