#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"
#include "mesh.h"
#include <sys/types.h>

struct Player
{
    struct Camera *cam;
    Vec3f vel;
};

struct Player *player_alloc();
void player_free(struct Player *p);

void player_move(struct Player *p, struct Mesh **solids, size_t nsolids);
void player_move_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids);

#endif

