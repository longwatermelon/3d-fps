#ifndef RENDER_H
#define RENDER_H

#include "util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Point render_project_point(Vec3f p);

Vec3f render_rotate_ccw(Vec3f p, Vec3f angle);
Vec3f render_rotate_cc(Vec3f p, Vec3f angle);

SDL_Texture *render_text(SDL_Renderer *rend, TTF_Font *font, const char *s);

void render_filled_tri(SDL_Point p[3], uint32_t *screen, SDL_Color col);
float render_fill_edges(float x1, float x2, int top, int bot, float s1, float s2, uint32_t *screen, SDL_Color col);

#endif

