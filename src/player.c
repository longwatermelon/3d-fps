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

