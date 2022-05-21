#include "player.h"
#include "camera.h"
#include "render.h"
#include <stdlib.h>


struct Player *player_alloc()
{
    struct Player *p = malloc(sizeof(struct Player));
    p->cam = cam_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f });
    p->vel = (Vec3f){ 0.f, 0.f, 0.f };

    p->gun = mesh_alloc((Vec3f){ .3f, .5f, .8f }, (Vec3f){ 0.f, 0.f, 0.f }, "res/gun.obj");

    return p;
}


void player_free(struct Player *p)
{
    cam_free(p->cam);
    mesh_free(p->gun);
    free(p);
}


void player_move(struct Player *p, struct Mesh **solids, size_t nsolids)
{
    p->vel.y += .0098f;

//    printf("%f\n", p->vel.y);

    Vec3f x = { p->vel.x, 0.f, 0.f };
    Vec3f y = { 0.f, p->vel.y, 0.f };
    Vec3f z = { 0.f, 0.f, p->vel.z };

    if (!player_move_dir(p, x, solids, nsolids, .5f)) p->vel.x = 0.f;

    if (!player_move_dir(p, y, solids, nsolids, 2.f)) p->vel.y = 0.f;

    if (!player_move_dir(p, z, solids, nsolids, .5f)) p->vel.z = 0.f;

    p->gun->pos = vec_addv(p->cam->pos, (Vec3f){ .3f, .5f, .8f });

    p->gun->pos = vec_addv(p->cam->pos, render_rotate_cc(vec_sub(p->gun->pos, p->cam->pos), p->cam->angle));
    p->gun->rot = p->cam->angle;
}


bool player_move_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float bound)
{
    float min;

    if (player_check_dir(p, dir, solids, nsolids, bound, &min))
    {
        p->cam->pos = vec_addv(p->cam->pos, dir);
        return true;
    }
    else
    {
        float dist;
        player_check_dir(p, (Vec3f){ 0.f, 0.f, 0.f }, solids, nsolids, bound, &dist);

        float diff = dist - bound;
        p->cam->pos = vec_addv(p->cam->pos, vec_mulf(vec_normalize(dir), diff));
        return false;
    }
}


bool player_check_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float bound, float *min)
{
    float m = INFINITY;
    Vec3f new = vec_addv(p->cam->pos, dir);

    for (size_t i = 0; i < nsolids; ++i)
    {
        float t = mesh_point_shortest_dist(solids[i], new);

        if (t < m)
            m = t;
    }

    if (min)
        *min = m;

    return m > bound;
}


void player_render(struct Player *p, SDL_Renderer *rend)
{
    mesh_render(p->gun, rend, p->cam);
}

