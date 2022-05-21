#include "enemy.h"


struct Enemy *enemy_alloc(Vec3f pos)
{
    struct Enemy *e = malloc(sizeof(struct Enemy));
    e->pos = pos;

    e->body[0] = mesh_alloc(e->pos, (Vec3f){ 1.f, 1.f, .8f }, "res/donut.obj", (SDL_Color){ 230, 150, 245 });
    e->body[1] = mesh_alloc(e->pos, (Vec3f){ .6f, 1.6f, 1.f }, "res/donut.obj", (SDL_Color){ 230, 150, 245 });

    e->health = 5;

    return e;
}


void enemy_free(struct Enemy *e)
{
    mesh_free(e->body[0]);
    mesh_free(e->body[1]);
    free(e);
}


void enemy_render(struct Enemy *e, SDL_Renderer *rend, struct Camera *c)
{
    e->body[0]->rot = vec_addv(e->body[0]->rot, (Vec3f){ .06f, .04f, .07f });
    e->body[1]->rot = vec_addv(e->body[1]->rot, (Vec3f){ -.09f, .08f, -.04f });

    mesh_render(e->body[0], rend, c);
    mesh_render(e->body[1], rend, c);
}


bool enemy_ray_intersect(struct Enemy *e, Vec3f o, Vec3f dir, float *t)
{
    float min = INFINITY;

    for (size_t i = 0; i < 2; ++i)
    {
        float tmp;
        Triangle tri;

        if (mesh_ray_intersect(e->body[i], o, dir, &tmp, &tri))
        {
            if (tmp < min)
                min = tmp;
        }
    }

    if (t)
        *t = min;

    return min != INFINITY;
}

