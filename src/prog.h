#ifndef PROG_H
#define PROG_H

#include "player.h"
#include "mesh.h"
#include "enemy.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

struct Prog
{
    bool running;

    SDL_Window *window;
    SDL_Renderer *rend;

    bool focused;

    struct Player *player;

    struct Mesh **solids;
    size_t nsolids;

    struct Enemy **enemies;
    size_t nenemies;
};

struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r);
void prog_free(struct Prog *p);

void prog_mainloop(struct Prog *p);
void prog_events(struct Prog *p, SDL_Event *evt);

bool prog_player_shoot(struct Prog *p, struct Enemy **e);

#endif

