#include "prog.h"
#include "render.h"
#include "mesh.h"
#include "enemy.h"
#include "audio.h"


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;
    p->restart = false;

    p->window = w;
    p->rend = r;

    p->font = TTF_OpenFont("res/font.ttf", 16);

    SDL_WarpMouseInWindow(p->window, 400, 400);
    SDL_ShowCursor(SDL_FALSE);
    p->focused = true;

    p->player = player_alloc();

    p->solids = 0;
    p->nsolids = 0;

    p->enemies = 0;
    p->nenemies = 0;

    p->score = 0;

    return p;
}


void prog_free(struct Prog *p)
{
    audio_stop_music();

    TTF_CloseFont(p->font);

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
    audio_play_music("res/sfx/wind.wav");
    SDL_Event evt;

    p->nsolids = 2;
    p->solids = malloc(sizeof(struct Mesh*) * p->nsolids);

    SDL_Color solid_col = { 170, 170, 170 };

    p->solids[0] = mesh_alloc((Vec3f){ 0.f, 5.f, 0.f }, (Vec3f){ .2f, .1f, .3f }, "res/plane.obj", solid_col);
    p->solids[0]->bculling = false;
    p->solids[1] = mesh_alloc((Vec3f){ 0.f, 0.f, 13.f }, (Vec3f){ .4f, .1f, .3f }, "res/big.obj", solid_col);

    while (p->running)
    {
        int prev_score = p->score;

        if (p->player->health > 0)
            prog_events(p, &evt);
        else
        {
            while (SDL_PollEvent(&evt))
                prog_events_base(p, &evt);
        }

        if (p->player->health > 0)
        {
            prog_mouse(p);

            prog_player(p);
            prog_enemies(p);
        }

        for (size_t i = 0; i < p->nsolids; ++i)
        {
            if (p->score >= 30)
                p->solids[i]->rot.z += .007f;
        }

        audio_stop_finished_sounds();

        SDL_RenderClear(p->rend);

        prog_render(p);

        if (p->player->health <= 0)
        {
            SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 100);
            SDL_SetRenderDrawBlendMode(p->rend, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(p->rend, 0);
            SDL_SetRenderDrawBlendMode(p->rend, SDL_BLENDMODE_NONE);
        }

        SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 255);
        SDL_RenderPresent(p->rend);

        if (p->score != prev_score && prev_score < 30 && p->score >= 30)
        {
            audio_play_music("res/sfx/roll.wav");
            audio_play_sound("res/sfx/rotate.wav");
        }
    }
}


void prog_events(struct Prog *p, SDL_Event *evt)
{
    while (SDL_PollEvent(evt))
    {
        prog_events_base(p, evt);
        prog_events_game(p, evt);
    }
}


void prog_events_base(struct Prog *p, SDL_Event *evt)
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
        case SDLK_q:
            p->restart = true;
            p->running = false;
            break;
        }
    } break;
    case SDL_MOUSEBUTTONDOWN:
        p->focused = true;
        SDL_ShowCursor(SDL_FALSE);
        break;
    }
}


void prog_events_game(struct Prog *p, SDL_Event *evt)
{
    switch (evt->type)
    {
    case SDL_KEYDOWN:
    {
        switch (evt->key.keysym.sym)
        {
        case SDLK_SPACE:
            if (p->player->vel.y == 0.f)
                p->player->vel.y = -.3f;
            break;
        case SDLK_1:
            player_switch_weapon(p->player, p->player->gun);
            break;
        case SDLK_2:
            player_switch_weapon(p->player, p->player->knife);
            break;
        }
    } break;
    case SDL_MOUSEBUTTONDOWN:
        if (evt->button.button == SDL_BUTTON_LEFT)
        {
            if (p->player->weapon == p->player->gun)
            {
                struct Enemy *e = 0;
                bool hit = prog_player_shoot(p, &e);
                p->player->gun->mesh->pos.y -= .1f;
                p->player->gun->mesh->rot.y += .2f;

                audio_play_sound("res/sfx/gunshot.wav");

                if (hit)
                    p->score += enemy_hurt(e, 1);
            }
            else if (p->player->weapon == p->player->knife)
            {
                audio_play_sound("res/sfx/slash.wav");
                p->player->knife->absolute = true;
                p->player->knife_thrown = true;
                p->player->knife->pos = vec_addv(p->player->cam->pos, render_rotate_cc((Vec3f){ 0.f, 0.f, 90.f }, p->player->cam->angle));
                p->player->knife->divisor = 10.f;
                p->player->knife_throw_origin = p->player->cam->pos;
            }
        }

        if (evt->button.button == SDL_BUTTON_RIGHT)
        {
            p->player->scoped = !p->player->scoped;

            if (p->player->weapon == p->player->gun)
                audio_play_sound("res/sfx/gun_scope.wav");
        }
        break;
    }

    prog_events_keystate(p);
}


void prog_events_keystate(struct Prog *p)
{
    struct Camera *cam = p->player->cam;
    const Uint8 *keys = SDL_GetKeyboardState(0);

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


void prog_mouse(struct Prog *p)
{
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
}


void prog_enemies(struct Prog *p)
{
    for (size_t i = 0; i < p->nenemies; ++i)
    {
        if (p->enemies[i]->dead && (clock() - p->enemies[i]->dead_time) / CLOCKS_PER_SEC >= 1)
        {
            enemy_free(p->enemies[i]);
            memmove(p->enemies + i, p->enemies + i + 1, (--p->nenemies - i) * sizeof(struct Enemy*));
        }

        if (p->enemies[i]->type == ENEMY_DODGE)
        {
            float t;
            Vec3f pdir = render_rotate_cc((Vec3f){ 0.f, 0.f, 1.f }, p->player->cam->angle);

            if (p->player->knife_thrown && enemy_ray_intersect(p->enemies[i], p->player->cam->pos, pdir, &t))
            {
                if (vec_dot(vec_sub(p->enemies[i]->body[0]->pos, p->player->cam->pos), pdir) >= 0.f)
                {
                    Vec3f dst = vec_addv(p->player->cam->pos, vec_mulf(pdir, -10.f));
                    enemy_move(p->enemies[i], p->rend, vec_sub(dst, p->enemies[i]->pos));
                }
            }
        }

        if (p->enemies[i]->type == ENEMY_THROW)
        {
            for (size_t j = 0; j < p->enemies[i]->nbody; ++j)
            {
                if (p->enemies[i]->thrown[j])
                    continue;

                if (rand() % 300 < 1)
                {
                    p->enemies[i]->thrown[j] = p->enemies[i]->body[j];
                    p->enemies[i]->thrown_vectors[j] = vec_divf(vec_normalize(vec_sub(p->player->cam->pos, p->enemies[i]->body[j]->pos)), 1.f);
                }
            }
        }
    }

    if (rand() % 300 < 5 && p->nenemies < 3)
    {
        p->enemies = realloc(p->enemies, sizeof(struct Enemy*) * ++p->nenemies);

        int type;
        int rng = rand() % 100;

        if (rng < 60)
            type = ENEMY_NORMAL;
        else if (rng < 70)
            type = ENEMY_DODGE;
        else
            type = ENEMY_THROW;

        p->enemies[p->nenemies - 1] = enemy_alloc((Vec3f){ rand() % 40 - 20, rand() % 40 - 20, rand() % 40 - 20 }, type);
    }

    for (size_t i = 0; i < p->nenemies; ++i)
    {
        Vec3f move;

        if (p->enemies[i]->type == ENEMY_THROW)
            move = (Vec3f){ 0.f, 0.f, 0.f };
        else
            move = vec_divf(vec_normalize(vec_sub(p->player->cam->pos, p->enemies[i]->pos)), 10.f);

        enemy_move(p->enemies[i], p->rend, move);
    }
}


void prog_player(struct Prog *p)
{
    player_move(p->player, p->solids, p->nsolids);

    for (size_t i = 0; i < p->nenemies; ++i)
    {
        for (size_t j = 0; j < p->enemies[i]->nbody; ++j)
        {
            if (vec_len(vec_sub(p->player->cam->pos, p->enemies[i]->body[j]->pos)) <= 2.f)
            {
                if (!p->enemies[i]->dead)
                    player_hurt(p->player, 1);
            }
        }
    }

    if (p->player->knife_thrown)
    {
        struct Weapon *w = p->player->weapon;
        float x = vec_len(vec_sub(w->pos, p->player->knife_throw_origin));
        float y = vec_len(vec_sub(w->pos, vec_addv(p->player->cam->pos, render_rotate_cc(w->default_pos, p->player->cam->angle))));
        float y1 = vec_len(vec_sub(w->pos, w->mesh->pos));

        float x1 = (x * y1) / y;
        float dist = x1;

        y1 = vec_len(vec_sub(vec_sub(w->pos, w->mesh->pos), vec_divf(vec_sub(w->pos, w->mesh->pos), w->divisor)));
        x1 = (x * y1) / y;

        dist -= x1;

        Vec3f dir = vec_normalize(vec_sub(w->pos, p->player->knife_throw_origin));

        for (size_t i = 0; i < p->nenemies; ++i)
        {
            if (p->enemies[i]->type == ENEMY_DODGE)
                continue;

            float t;

            if (enemy_ray_intersect(p->enemies[i], p->player->knife_throw_origin, dir, &t))
            {
                if (t <= dist)
                    p->score += enemy_hurt(p->enemies[i], 5);
            }
        }

        p->player->knife_throw_origin = vec_addv(p->player->knife_throw_origin, vec_mulf(dir, dist));
    }
}


void prog_render(struct Prog *p)
{
    for (size_t i = 0; i < p->nsolids; ++i)
        mesh_render(p->solids[i], p->rend, p->player->cam);

    for (size_t i = 0; i < p->nenemies; ++i)
        enemy_render(p->enemies[i], p->rend, p->player->cam);

    player_render(p->player, p->rend, p->font);

    if (p->player->scoped)
    {
        SDL_SetRenderDrawColor(p->rend, 255, 255, 255, 255);
        SDL_RenderDrawLine(p->rend, 400 - 10, 400 - 10, 400 + 10, 400 + 10);
        SDL_RenderDrawLine(p->rend, 400 + 10, 400 - 10, 400 - 10, 400 + 10);
    }

    char score[100] = { 0 };
    sprintf(score, "Score: %d", p->score);

    SDL_Texture *tex = render_text(p->rend, p->font, score);
    SDL_Rect r = { 20, 60 };
    SDL_QueryTexture(tex, 0, 0, &r.w, &r.h);

    SDL_RenderCopy(p->rend, tex, 0, &r);
    SDL_DestroyTexture(tex);
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

