#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"

struct Player
{
    struct Camera *cam;
};

struct Player *player_alloc();
void player_free(struct Player *p);

#endif

