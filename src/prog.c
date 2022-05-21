#include "prog.h"
#include "render.h"
#include "mesh.h"
#include "enemy.h"


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

    p->enemies = 0;
    p->nenemies = 0;

    return p;
}


void prog_free(struct Prog *p)
{
    player_free(p->player);

    for (size_t i = 0; i < p->nsolids; ++i)
        mesh_free(p->solids[i]);

    free(p->solids);

    for (size_t i = 0; i < p->nenemies; ++i)
        enemy_free(p->enemies[i]);

    free(p->enemies);

    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    p->nsolids = 2;
    p->solids = malloc(sizeof(struct Mesh*) * p->nsolids);

    p->nenemies = 1;
    p->enemies = malloc(sizeof(struct Enemy*) * p->nenemies);

    SDL_Color solid_col = { 200, 200, 200 };

    p->solids[0] = mesh_alloc((Vec3f){ 0.f, 5.f, 0.f }, (Vec3f){ .2f, .1f, .3f }, "res/plane.obj", solid_col);
    p->solids[0]->bculling = false;
    p->solids[1] = mesh_alloc((Vec3f){ 0.f, 0.f, 13.f }, (Vec3f){ .4f, .1f, .3f }, "res/big.obj", solid_col);

    p->enemies[0] = enemy_alloc((Vec3f){ 0.f, 0.f, 5.f });

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

        for (size_t i = 0; i < p->nenemies; ++i)
        {
            if (p->enemies[i]->health <= 0)
            {
                enemy_free(p->enemies[i]);
                memmove(p->enemies + i, p->enemies + i + 1, (--p->nenemies - i) * sizeof(struct Enemy*));
            }
        }

        if (rand() % 500 < 1)
        {
            p->enemies = realloc(p->enemies, sizeof(struct Enemy*) * ++p->nenemies);
            p->enemies[p->nenemies - 1] = enemy_alloc((Vec3f){ rand() % 40 - 20, rand() % 40 - 20, rand() % 40 - 20 });
        }

        for (size_t i = 0; i < p->nenemies; ++i)
        {
            enemy_move(p->enemies[i], p->rend, vec_divf(vec_normalize(vec_sub(p->player->cam->pos, p->enemies[i]->pos)), 10.f));
        }

        player_move(p->player, p->solids, p->nsolids);

        SDL_RenderClear(p->rend);

        for (size_t i = 0; i < p->nsolids; ++i)
            mesh_render(p->solids[i], p->rend, p->player->cam);

        for (size_t i = 0; i < p->nenemies; ++i)
            enemy_render(p->enemies[i], p->rend, p->player->cam);

        player_render(p->player, p->rend);

        if (p->player->scoped)
        {
            SDL_SetRenderDrawColor(p->rend, 255, 255, 255, 255);
            SDL_RenderDrawLine(p->rend, 400 - 10, 400 - 10, 400 + 10, 400 + 10);
            SDL_RenderDrawLine(p->rend, 400 + 10, 400 - 10, 400 - 10, 400 + 10);
        }

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

            if (evt->button.button == SDL_BUTTON_LEFT)
            {
                struct Enemy *e;
                bool hit = prog_player_shoot(p, &e);
                p->player->gun->pos.y -= .1f;
                p->player->gun->rot.y += .2f;

                if (hit)
                {
                    --e->health;
                    SDL_Color red = { 255, 0, 0 };

                    e->body[0]->col = red;
                    e->body[1]->col = red;
                }
            }

            if (evt->button.button == SDL_BUTTON_RIGHT)
            {
                p->player->scoped = !p->player->scoped;
            }
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(0);

    Vec3f move = { 0.f, 0.f, 0.f };
    float speed = .2f;

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

    p->player->vel.x = move.x;
    p->player->vel.z = move.z;
}


bool prog_player_shoot(struct Prog *p, struct Enemy **e)
{
    Vec3f dir = render_rotate_cc((Vec3f){ 0.f, 0.f, 1.f }, p->player->cam->angle);
    float et = INFINITY, st = INFINITY;

    for (size_t i = 0; i < p->nenemies; ++i)
    {
        float t;

        if (enemy_ray_intersect(p->enemies[i], p->player->cam->pos, dir, &t))
        {
            if (t < et)
            {
                et = t;
                if (e) *e = p->enemies[i];
            }
        }
    }

    for (size_t i = 0; i < p->nsolids; ++i)
    {
        float t;
        Triangle tri;

        if (mesh_ray_intersect(p->solids[i], p->player->cam->pos, dir, &t, &tri))
        {
            if (t < st)
                st = t;
        }
    }

    return et < st;
}

