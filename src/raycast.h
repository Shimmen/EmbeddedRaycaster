#ifndef RAYCAST_H_
#define RAYCAST_H_

#include "math.h"
#include "map.h"

typedef struct camera {
    Vec2 pos;
    Vec2 dir;
    Vec2 plane; // The 2d raycaster version of camera plane
} Camera;

typedef struct ray {
    Vec2 pos;
    Vec2 dir;
} Ray;

//
// Get ray from camera
//
Ray get_ray_from_camera(Camera camera, int x_row, int window_width);

typedef struct {
    int wall_bottom;
    int wall_top;
    int wall_type;
} RayHit;

//
// Perform raycast
//
RayHit perform_raycast(const Map *map, Ray ray);

#endif // RAYCAST_H_