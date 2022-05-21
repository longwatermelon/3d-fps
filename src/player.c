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
    Vec3f x = { p->vel.x, 0.f, 0.f };
    Vec3f y = { 0.f, p->vel.y, 0.f };
    Vec3f z = { 0.f, 0.f, p->vel.z };

    player_move_dir(p, x, solids, nsolids);
    player_move_dir(p, y, solids, nsolids);
    player_move_dir(p, z, solids, nsolids);
}


void player_move_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids)
{
    float min = INFINITY;
    Vec3f new = vec_addv(p->cam->pos, dir);

    for (size_t i = 0; i < nsolids; ++i)
    {
        float t = mesh_point_shortest_dist(solids[i], new);

        if (t < min)
            min = t;
    }

    if (min > vec_len(dir) + .5f)
        p->cam->pos = new;
}

