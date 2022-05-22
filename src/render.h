#ifndef RENDER_H
#define RENDER_H

#include "util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Point render_project_point(Vec3f p);

Vec3f render_rotate_ccw(Vec3f p, Vec3f angle);
Vec3f render_rotate_cc(Vec3f p, Vec3f angle);

SDL_Texture *render_text(SDL_Renderer *rend, TTF_Font *font, const char *s);

#endif

