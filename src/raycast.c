#include "raycast.h"

#include <math.h>

#include "rendering.h"

inline Vec2 vec2(float x, float y)
{
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

Ray get_ray_from_camera(Camera camera, int x_row, int window_width)
{
    Ray ray;

    float camera_x = 2 * x_row / (float)window_width - 1.0f; // x-coordinate in camera space

    ray.pos = camera.pos;
    ray.dir = vec2(
            camera.dir.x + camera.plane.x * camera_x,
            camera.dir.y + camera.plane.y * camera_x
    );

    return ray;
}

/*
Copyright (c) 2004-2007, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
RayHit perform_raycast(const Map *map, Ray ray)
{
    // Which box of the map we're in
    int mapX = (int)ray.pos.x;
    int mapY = (int)ray.pos.y;

    // Length of ray from current position to next x or y-side
    double side_dist_x;
    double side_dist_y;

    // Length of ray from one x or y-side to next x or y-side
    double delta_dist_x = sqrt(1 + (ray.dir.y * ray.dir.y) / (ray.dir.x * ray.dir.x));
    double delta_dist_y = sqrt(1 + (ray.dir.x * ray.dir.x) / (ray.dir.y * ray.dir.y));
    double perp_wall_dist;

    // What direction to step in x or y-direction (either +1 or -1)
    int step_x;
    int step_y;

    int hit = 0; // Was there a wall hit?
    int side = -1; // Was a NS or a EW wall hit?

    // Calculate step and initial sideDist
    if (ray.dir.x < 0)
    {
        step_x = -1;
        side_dist_x = (ray.pos.x - mapX) * delta_dist_x;
    }
    else
    {
        step_x = 1;
        side_dist_x = (mapX + 1.0 - ray.pos.x) * delta_dist_x;
    }
    if (ray.dir.y < 0)
    {
        step_y = -1;
        side_dist_y = (ray.pos.y - mapY) * delta_dist_y;
    }
    else
    {
        step_y = 1;
        side_dist_y = (mapY + 1.0 - ray.pos.y) * delta_dist_y;
    }

    // Perform DDA
    while (hit == 0)
    {
        // Jump to next map square, OR in x-direction, OR in y-direction
        if (side_dist_x < side_dist_y)
        {
            side_dist_x += delta_dist_x;
            mapX += step_x;
            side = 0;
        }
        else
        {
            side_dist_y += delta_dist_y;
            mapY += step_y;
            side = 1;
        }

        // Check if ray has hit a wall
        if (get_tile(map, mapX, mapY) > 0) hit = 1;
    }

    // Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
    if (side == 0) perp_wall_dist = (mapX - ray.pos.x + (1 - step_x) / 2) / ray.dir.x;
    else           perp_wall_dist = (mapY - ray.pos.y + (1 - step_y) / 2) / ray.dir.y;

    // Calculate height of line to draw on screen
    int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);

    RayHit ray_hit;

    // Calculate lowest and highest pixel to fill in current stripe
    ray_hit.wall_bottom = -line_height / 2 + SCREEN_HEIGHT / 2;
    ray_hit.wall_top = line_height / 2 + SCREEN_HEIGHT / 2;

    return ray_hit;
}