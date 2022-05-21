#include "player.h"
#include "camera.h"
#include <stdlib.h>


struct Player *player_alloc()
{
    struct Player *p = malloc(sizeof(struct Player));
    p->cam = cam_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f });
    p->vel = (Vec3f){ 0.f, 0.f, 0.f };

    return p;
}


void player_free(struct Player *p)
{
    cam_free(p->cam);
    free(p);
}


void player_move(struct Player *p, struct Mesh **solids, size_t nsolids)
{
    p->vel.y += .0098f;

//    printf("%f\n", p->vel.y);

    Vec3f x = { p->vel.x, 0.f, 0.f };
    Vec3f y = { 0.f, p->vel.y, 0.f };
    Vec3f z = { 0.f, 0.f, p->vel.z };

    if (!player_move_dir(p, x, solids, nsolids)) p->vel.x = 0.f;
    if (!player_move_dir(p, y, solids, nsolids)) p->vel.y = 0.f;
    if (!player_move_dir(p, z, solids, nsolids)) p->vel.z = 0.f;
}


bool player_move_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids)
{
    float min;

    if (player_check_dir(p, dir, solids, nsolids, &min))
    {
        p->cam->pos = vec_addv(p->cam->pos, dir);
        return true;
    }
    else
    {
        return false;
    }
}


bool player_check_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float *min)
{
    float m = INFINITY;
    Vec3f new = vec_addv(p->cam->pos, dir);
//    vec_print(dir);

    for (size_t i = 0; i < nsolids; ++i)
    {
        float t = mesh_point_shortest_dist(solids[i], new);

        if (t < m)
            m = t;
    }

    if (min)
        *min = m;

//    printf("m = %f\n", m);
    return m > .5f;
}

