#include "input.h"

#include <stdbool.h>

#include <SDL2/SDL.h>

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
