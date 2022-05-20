#include "prog.h"
#include "mesh.h"


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;

    p->window = w;
    p->rend = r;

    p->player = player_alloc();

    return p;
}


void prog_free(struct Prog *p)
{
    player_free(p->player);
    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    struct Mesh *m = mesh_alloc((Vec3f){ 0.f, 0.f, 5.f }, "res/donut.obj");

    while (p->running)
    {
        prog_events(p, &evt);

        SDL_RenderClear(p->rend);

        mesh_render(m, p->rend, p->player->cam);

        SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 255);
        SDL_RenderPresent(p->rend);
    }

    mesh_free(m);
}


void prog_events(struct Prog *p, SDL_Event *evt)
{
    struct Camera *cam = p->player->cam;

    while (SDL_PollEvent(evt))
    {
        switch (evt->type)
        {
        case SDL_QUIT:
            p->running = false;
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(0);

    if (keys[SDL_SCANCODE_W])
    {
        cam->pos.z += .1f * cosf(cam->angle.x);
        cam->pos.x += .1f * sinf(cam->angle.x);
    }

    if (keys[SDL_SCANCODE_S])
    {
        cam->pos.z -= .1f * cosf(cam->angle.x);
        cam->pos.x -= .1f * sinf(cam->angle.x);
    }

    if (keys[SDL_SCANCODE_A])
    {
        cam->pos.x += .1f * sinf(-M_PI / 2.f + cam->angle.x);
        cam->pos.z += .1f * cosf(-M_PI / 2.f + cam->angle.x);
    }

    if (keys[SDL_SCANCODE_D])
    {
        cam->pos.x -= .1f * sinf(-M_PI / 2.f + cam->angle.x);
        cam->pos.z -= .1f * cosf(-M_PI / 2.f + cam->angle.x);
    }

    if (keys[SDL_SCANCODE_SPACE]) cam->pos.y -= .1f;
    if (keys[SDL_SCANCODE_LSHIFT]) cam->pos.y += .1f;
}

