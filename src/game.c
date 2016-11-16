
// TODO: Check which ones are available on the MD407
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include "map.h"
#include "input.h"
#include "timing.h"
#include "raycast.h"
#include "rendering.h"

int main()
{
    Camera camera;
    camera.pos = vec2(22, 12);
    camera.dir = vec2(-1, 0);
    camera.plane = vec2(0.0f, 0.66f);

    screen_setup();

    const Map *map = &default_map;
    InputData input_data = { 0, 0, 0 };

    unsigned int accumulated_frame_time = 0;
    unsigned int frame_count = 0;

    unsigned int frame_time = 0;
    unsigned int current_time = get_current_time();
    unsigned int last_time = current_time;

    while(!input_data.should_exit)
    {
        frame_time = current_time - last_time;
        last_time = current_time;
        current_time = get_current_time();

        // Camera movement & rotation
        {
            get_input(&input_data);

            float frame_time_seconds = frame_time / 1000.0f;

            float move_speed = frame_time_seconds * 5.0f * input_data.y_axis; // squares/second
            float rot_speed = frame_time_seconds * 3.0f * input_data.x_axis; // radians/second

            int current_x = (int)camera.pos.x;
            int current_y = (int)camera.pos.y;

            int new_x = (int)(camera.pos.x + camera.dir.x * move_speed);
            int new_y = (int)(camera.pos.y + camera.dir.y * move_speed);

            // Move camera (if not obstructed)
            if (get_tile(map, new_x, current_y) == 0) camera.pos.x += camera.dir.x * move_speed;
            if (get_tile(map, current_x, new_y) == 0) camera.pos.y += camera.dir.y * move_speed;

            // Rotate (both camera direction and camera plane must be rotated)
            float old_dir_x = camera.dir.x;
            camera.dir.x = camera.dir.x * cosf(-rot_speed) - camera.dir.y * sinf(-rot_speed);
            camera.dir.y = old_dir_x * sinf(-rot_speed) + camera.dir.y * cosf(-rot_speed);
            float old_plane_x = camera.plane.x;
            camera.plane.x = camera.plane.x * cosf(-rot_speed) - camera.plane.y * sinf(-rot_speed);
            camera.plane.y = old_plane_x * sinf(-rot_speed) + camera.plane.y * cosf(-rot_speed);
        }

        screen_clear();

        // Perform ray-casting for all columns/pixels on the window
        for(int x = 0; x < SCREEN_WIDTH; x += 1)
        {
            Ray ray = get_ray_from_camera(camera, x, SCREEN_WIDTH);
            RayHit ray_hit = perform_raycast(map, ray);
            draw_column(x, ray_hit.wall_bottom, ray_hit.wall_top, false);
        }

        screen_push_rendered();

        // Logging
        frame_count += 1;
        accumulated_frame_time += frame_time;
        if (accumulated_frame_time > 1000)
        {
            float average_frame_time = (float)accumulated_frame_time / (float)frame_count;
            int rounded_frame_time = (int)(average_frame_time + 0.5f);
            int fps = (int) (1000.0f / average_frame_time);
            printf("Frame time: %dms, FPS: %d\n", rounded_frame_time, fps);
            accumulated_frame_time -= 1000;
            frame_count = 0;
        }
    }
}
