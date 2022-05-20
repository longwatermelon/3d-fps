#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

struct Camera
{
    Vec3f pos, angle;
};

struct Camera *cam_alloc(Vec3f pos, Vec3f angle);
void cam_free(struct Camera *c);

#endif

