#ifndef TIMING_H_
#define TIMING_H_

#include <SDL2/SDL.h>

unsigned int static inline get_current_time()
{
    return SDL_GetTicks();
}

#endif // TIMING_H_