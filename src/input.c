#include "input.h"

#if !EMBEDDED_MODE

#include <stdbool.h>

#include <SDL2/SDL.h>

void input_init(InputData *input_data)
{
    // Do nothing...
}

void get_input(InputData *input_data)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            input_data->should_exit = true;
            return;
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            int val = (event.type == SDL_KEYDOWN) ? 1 : 0;
            switch (event.key.keysym.sym)
            {
                case SDLK_LEFT: input_data->left = val; break;
                case SDLK_RIGHT: input_data->right = val; break;
                case SDLK_UP: input_data->forward = val; break;
                case SDLK_DOWN: input_data->backward = val;break;
                default: break;
            }

            input_data->x_axis = input_data->right - input_data->left;
            input_data->y_axis = input_data->forward - input_data->backward;
        }
    }
}

#else // if EMBEDDED_MODE

#include <stdint.h>

#include "gpio.h"
#include "keypad.h"

GPIO *gpio_d;
Keypad keypad;

void input_init(InputData *input_data)
{
    gpio_d = gpio_create(GPIO_D);
    keypad = keypad_create(gpio_d, GPIO_SECTION_HIGH, false);
}

void get_input(InputData *input_data)
{
    uint8_t key = keypad_get(&keypad);
    switch (key) {

        case 2:
            input_data->y_axis = 1;
            break;

        case 5:
            input_data->y_axis = -1;
            break;

        case 4:
            input_data->x_axis = -1;
            break;

        case 6:
            input_data->x_axis = 1;
            break;

        case 0xFF:
        default:
            input_data->y_axis = 0;
            input_data->x_axis = 0;
    }
}

#endif