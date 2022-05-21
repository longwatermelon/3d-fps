#ifndef ENEMY_H
#define ENEMY_H

#include "util.h"
#include "mesh.h"

struct Enemy
{
    Vec3f pos;
    struct Mesh *body[2];

    int health;
};

struct Enemy *enemy_alloc(Vec3f pos);
void enemy_free(struct Enemy *e);

void enemy_render(struct Enemy *e, SDL_Renderer *rend, struct Camera *c);
void enemy_move(struct Enemy *e, SDL_Renderer *rend, Vec3f v);

bool enemy_ray_intersect(struct Enemy *e, Vec3f o, Vec3f dir, float *t);

#endif

