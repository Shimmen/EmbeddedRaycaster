
#include <math.h>
#include <stdbool.h>

#include "map.h"
#include "input.h"
#include "config.h"
#include "raycast.h"
#include "rendering.h"

//#include "timer.h"

#if !EMBEDDED_MODE
 // timing.h only for when running on a PC. Embedded mode is predictable enough to hard-code a timing value
 #include "timing.h"
#endif

#if EMBEDDED_MODE
void startup() __attribute__((naked)) __attribute__((section (".start_section")));
void startup()
{
    asm volatile(
        " LDR R0,=0x2001C000\n" /* set stack */
        " MOV SP,R0\n"
        " BL main\n"            /* call main */
        ".L1: B .L1\n"          /* never return */
    );
}
#endif

void init_camera_for_map(Camera *camera, const Map *map)
{
    camera->pos = map->start_pos;
    camera->dir = map->start_dir;
    camera->plane = map->start_camera_plane;
}

int main()
{
    screen_setup();

    const Map *map = &default_map;
    InputData input_data = { 0, 0, 0, 0, 0, 0, false, false };
    input_init(&input_data);

    Camera camera;
    init_camera_for_map(&camera, map);

#if !EMBEDDED_MODE
    unsigned int frame_time = 0;
    unsigned int current_time = get_current_time();
    unsigned int last_time = current_time;
#endif

    while(!input_data.should_exit)
    {
#if !EMBEDDED_MODE
        frame_time = current_time - last_time;
        last_time = current_time;
        current_time = get_current_time();
#else
        const unsigned int frame_time = 30;
#endif

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

            if (input_data.should_teleport)
            {
                input_data.should_teleport = false;
                // TODO: Check if in range for teleport, and add logic for choosing map!
                map = &line_map;
                init_camera_for_map(&camera, map);
            }
        }

        screen_clear();

        // Perform ray-casting for all columns/pixels on the window
        for(int x = 0; x < SCREEN_WIDTH; x += 1)
        {
            Ray ray = get_ray_from_camera(camera, x, SCREEN_WIDTH);
            RayHit ray_hit = perform_raycast(map, ray);
            draw_column(x, ray_hit.wall_bottom, ray_hit.wall_top, ray_hit.wall_type == 2);
        }

        screen_push_rendered();
    }
}
