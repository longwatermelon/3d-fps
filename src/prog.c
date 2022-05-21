#include "prog.h"
#include "mesh.h"


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;

    p->window = w;
    p->rend = r;

    p->focused = false;

    p->player = player_alloc();

    p->solids = 0;
    p->nsolids = 0;

    return p;
}


void prog_free(struct Prog *p)
{
    player_free(p->player);

    for (size_t i = 0; i < p->nsolids; ++i)
        mesh_free(p->solids[i]);

    free(p->solids);

    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    p->solids = realloc(p->solids, sizeof(struct Mesh) * ++p->nsolids);
    p->solids[0] = mesh_alloc((Vec3f){ 0.f, 0.f, 5.f }, "res/donut.obj");

    while (p->running)
    {
        prog_events(p, &evt);

        SDL_Point mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);

        if (p->focused)
        {
            SDL_Point diff = {
                mouse.x - 400,
                mouse.y - 400
            };

            SDL_WarpMouseInWindow(p->window, 400, 400);

            p->player->cam->angle.x += (float)diff.x / 200.f;
            p->player->cam->angle.y -= (float)diff.y / 200.f;
        }

        player_move(p->player, p->solids, p->nsolids);

        SDL_RenderClear(p->rend);

        for (size_t i = 0; i < p->nsolids; ++i)
            mesh_render(p->solids[i], p->rend, p->player->cam);

        SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 255);
        SDL_RenderPresent(p->rend);
    }
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
        case SDL_KEYDOWN:
        {
            switch (evt->key.keysym.sym)
            {
            case SDLK_ESCAPE:
                p->focused = false;
                SDL_ShowCursor(SDL_TRUE);
                break;
            }
        } break;
        case SDL_MOUSEBUTTONDOWN:
            p->focused = true;
            SDL_ShowCursor(SDL_FALSE);
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(0);

    Vec3f move = { 0.f, 0.f, 0.f };

    if (keys[SDL_SCANCODE_W])
    {
        move.z += .1f * cosf(cam->angle.x);
        move.x += .1f * sinf(cam->angle.x);
    }

    if (keys[SDL_SCANCODE_S])
    {
        move.z -= .1f * cosf(cam->angle.x);
        move.x -= .1f * sinf(cam->angle.x);
    }

    if (keys[SDL_SCANCODE_A])
    {
        move.x += .1f * sinf(-M_PI / 2.f + cam->angle.x);
        move.z += .1f * cosf(-M_PI / 2.f + cam->angle.x);
    }

    if (keys[SDL_SCANCODE_D])
    {
        move.x -= .1f * sinf(-M_PI / 2.f + cam->angle.x);
        move.z -= .1f * cosf(-M_PI / 2.f + cam->angle.x);
    }

    if (keys[SDL_SCANCODE_SPACE]) move.y -= .1f;
    if (keys[SDL_SCANCODE_LSHIFT]) move.y += .1f;

    p->player->vel = move;
}

