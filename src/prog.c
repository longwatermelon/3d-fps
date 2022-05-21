#include "prog.h"
#include "render.h"
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
    p->solids = realloc(p->solids, sizeof(struct Mesh) * ++p->nsolids);
    p->solids[0] = mesh_alloc((Vec3f){ 0.f, 5.f, 0.f }, "res/plane.obj");
    p->solids[1] = mesh_alloc((Vec3f){ 0.f, 0.f, 13.f }, "res/big.obj");
//    p->solids[0] = mesh_alloc((Vec3f){ 0.f, 5.f, 0.f }, "res/cube.obj");
//    p->solids[1] = mesh_alloc((Vec3f){ -2.f, 3.f, 0.f }, "res/cube.obj");

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

            Vec3f diff_a = {
                (float)diff.x / 200.f,
                -(float)diff.y / 200.f,
                0.f
            };

            p->player->cam->angle = vec_addv(p->player->cam->angle, diff_a);
        }

        player_move(p->player, p->solids, p->nsolids);

        SDL_RenderClear(p->rend);

        for (size_t i = 0; i < p->nsolids; ++i)
            mesh_render(p->solids[i], p->rend, p->player->cam);

        player_render(p->player, p->rend);

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
            case SDLK_SPACE:
                if (p->player->vel.y == 0.f)
                    p->player->vel.y = -.3f;
                break;
            case SDLK_LSHIFT:
                p->player->cam->pos.y += 10.f;
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
    float speed = .1f;

    if (keys[SDL_SCANCODE_W])
    {
        move.z += speed * cosf(cam->angle.x);
        move.x += speed * sinf(cam->angle.x);
    }

    if (keys[SDL_SCANCODE_S])
    {
        move.z -= speed * cosf(cam->angle.x);
        move.x -= speed * sinf(cam->angle.x);
    }

    if (keys[SDL_SCANCODE_A])
    {
        move.x += speed * sinf(-M_PI / 2.f + cam->angle.x);
        move.z += speed * cosf(-M_PI / 2.f + cam->angle.x);
    }

    if (keys[SDL_SCANCODE_D])
    {
        move.x -= speed * sinf(-M_PI / 2.f + cam->angle.x);
        move.z -= speed * cosf(-M_PI / 2.f + cam->angle.x);
    }

#if 0
    if (keys[SDL_SCANCODE_SPACE]) move.y -= speed;
    if (keys[SDL_SCANCODE_LSHIFT]) move.y += speed;
#endif

    p->player->vel.x = move.x;
    p->player->vel.z = move.z;
}

