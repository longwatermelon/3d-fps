#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"
#include "mesh.h"
#include <sys/types.h>

struct Player
{
    struct Camera *cam;
    Vec3f vel;

    struct Mesh *gun;
};

struct Player *player_alloc();
void player_free(struct Player *p);

void player_move(struct Player *p, struct Mesh **solids, size_t nsolids);
bool player_move_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float bound);
bool player_check_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float bound, float *min);

void player_render(struct Player *p, SDL_Renderer *rend);

#endif

