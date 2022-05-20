#ifndef RENDER_H
#define RENDER_H

#include "util.h"
#include <SDL2/SDL.h>

SDL_Point render_project_point(Vec3f p);

Vec3f render_rotate_ccw(Vec3f p, Vec3f angle);
Vec3f render_rotate_cc(Vec3f p, Vec3f angle);

#endif

