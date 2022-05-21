#include "player.h"
#include "camera.h"
#include <stdlib.h>


struct Player *player_alloc()
{
    struct Player *p = malloc(sizeof(struct Player));
    p->cam = cam_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f });

    return p;
}


void player_free(struct Player *p)
{
    cam_free(p->cam);
    free(p);
}


void player_move(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids)
{
#if 0
    for (size_t i = 0; i < nsolids; ++i)
    {
        float t;
        Triangle tri;

        if (mesh_ray_intersect(solids[i], p->cam->pos, vec_normalize(dir), &t, &tri))
        {
            if (t < vec_len(dir) + 1.f)
            {
                move = false;
                break;
            }
        }
    }

    if (move)
        p->cam->pos = vec_addv(p->cam->pos, dir);
#endif

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

//    printf("%f %f %f\n", p->cam->pos.x, p->cam->pos.y, p->cam->pos.z);
}

