#include "player.h"
#include "camera.h"
#include "render.h"
#include <stdlib.h>


struct Player *player_alloc()
{
    struct Player *p = malloc(sizeof(struct Player));
    p->cam = cam_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f });
    p->vel = (Vec3f){ 0.f, 0.f, 0.f };

    p->gun = mesh_alloc((Vec3f){ .3f, .5f, .8f }, (Vec3f){ 0.f, 0.f, 0.f }, "res/gun.obj", (SDL_Color){ 255, 255, 255 });
    p->scoped = false;

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

    Vec3f x = { p->vel.x, 0.f, 0.f };
    Vec3f y = { 0.f, p->vel.y, 0.f };
    Vec3f z = { 0.f, 0.f, p->vel.z };

    if (p->scoped)
    {
        x = vec_mulf(x, .25f);
        z = vec_mulf(z, .25f);
    }

    if (!player_move_dir(p, x, solids, nsolids, .5f)) p->vel.x = 0.f;
    if (!player_move_dir(p, y, solids, nsolids, 2.f)) p->vel.y = 0.f;
    if (!player_move_dir(p, z, solids, nsolids, .5f)) p->vel.z = 0.f;

    player_animate_gun(p);
}


bool player_move_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float bound)
{
    float min;
    Vec3f diff = p->cam->pos;
    bool move;

    if (player_check_dir(p, dir, solids, nsolids, bound, &min))
    {
        p->cam->pos = vec_addv(p->cam->pos, dir);
        move = true;
    }
    else
    {
        float dist;
        player_check_dir(p, (Vec3f){ 0.f, 0.f, 0.f }, solids, nsolids, bound, &dist);

        float diff = dist - bound;
        p->cam->pos = vec_addv(p->cam->pos, vec_mulf(vec_normalize(dir), diff));
        move = false;
    }

    diff = vec_sub(p->cam->pos, diff);
    p->gun->pos = vec_addv(p->gun->pos, diff);

    return move;
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


void player_animate_gun(struct Player *p)
{
    Vec3f scoped = vec_addv(p->cam->pos, render_rotate_cc((Vec3f){ 0.f, .4f, .6f }, p->cam->angle));
    Vec3f normal = vec_addv(p->cam->pos, render_rotate_cc((Vec3f){ .35f, .5f, .8f }, p->cam->angle));

    if (p->scoped)
        p->gun->pos = vec_addv(p->gun->pos, vec_divf(vec_sub(scoped, p->gun->pos), 5.f));
    else
        p->gun->pos = vec_addv(p->gun->pos, vec_divf(vec_sub(normal, p->gun->pos), 5.f));

    p->gun->rot = vec_addv(p->gun->rot, vec_divf(vec_sub(p->cam->angle, p->gun->rot), 5.f));
}

