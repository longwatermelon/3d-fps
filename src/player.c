#include "player.h"
#include "camera.h"
#include "render.h"
#include "audio.h"
#include <stdlib.h>


struct Player *player_alloc()
{
    struct Player *p = malloc(sizeof(struct Player));
    p->cam = cam_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f });
    p->vel = (Vec3f){ 0.f, 0.f, 0.f };

    p->gun = weapon_alloc("res/gun.obj", (Vec3f){ .35f, .5f, .8f }, (Vec3f){ 0.f, 0.f, 0.f }, 5.f);
    p->scoped = false;

    p->knife = weapon_alloc("res/knife.obj", (Vec3f){ .35f, -.1f, .6f }, (Vec3f){ 0.f, 0.f, 0.f }, 5.f);
    p->knife_thrown = false;
    p->knife_throw_origin = (Vec3f){ 0.f, 0.f, 0.f };

    p->weapon = p->gun;

    p->health = 5;
    p->last_hurt = 0;

    return p;
}


void player_free(struct Player *p)
{
    cam_free(p->cam);

    weapon_free(p->gun);
    weapon_free(p->knife);

    free(p);
}


void player_move(struct Player *p, struct Mesh **solids, size_t nsolids)
{
    p->vel.y += .0098f;

    Vec3f x = { p->vel.x, 0.f, 0.f };
    Vec3f y = { 0.f, p->vel.y, 0.f };
    Vec3f z = { 0.f, 0.f, p->vel.z };

    if (!player_move_dir(p, x, solids, nsolids, .5f)) p->vel.x = 0.f;
    if (!player_move_dir(p, y, solids, nsolids, 2.f)) p->vel.y = 0.f;
    if (!player_move_dir(p, z, solids, nsolids, .5f)) p->vel.z = 0.f;

    weapon_move(p->weapon, p->cam);
    player_animate_weapon(p);
}


bool player_move_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float bound)
{
    float min;
    Vec3f diff = p->cam->pos;
    bool move;

    if (player_check_dir(p, dir, solids, nsolids, bound, &min))
    {
        p->cam->pos = vec_addv(p->cam->pos, dir);
        move = true;
    }
    else
    {
        float dist;
        player_check_dir(p, (Vec3f){ 0.f, 0.f, 0.f }, solids, nsolids, bound, &dist);

        float diff = dist - bound;
        p->cam->pos = vec_addv(p->cam->pos, vec_mulf(vec_normalize(dir), diff));
        move = false;
    }

    if (!p->knife_thrown)
    {
        diff = vec_sub(p->cam->pos, diff);
        p->weapon->mesh->pos = vec_addv(p->weapon->mesh->pos, diff);
    }

    return move;
}


bool player_check_dir(struct Player *p, Vec3f dir, struct Mesh **solids, size_t nsolids, float bound, float *min)
{
    float m = INFINITY;
    Vec3f new = vec_addv(p->cam->pos, dir);

    for (size_t i = 0; i < nsolids; ++i)
    {
        float t = mesh_point_shortest_dist(solids[i], new);

        if (t < m)
            m = t;
    }

    if (min)
        *min = m;

    return m > bound;
}


void player_render(struct Player *p, SDL_Renderer *rend, TTF_Font *font)
{
    weapon_render(p->weapon, rend, p->cam);

    if (clock() - p->last_hurt < CLOCKS_PER_SEC && clock() > CLOCKS_PER_SEC)
    {
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rend, 255, 0, 0, (1.f - (float)(clock() - p->last_hurt) / (float)CLOCKS_PER_SEC) * 255.f);
        SDL_RenderFillRect(rend, 0);
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    }

    char s[100] = { 0 };
    sprintf(s, "Health: %d", p->health);

    SDL_Texture *tex = render_text(rend, font, s);
    SDL_Rect r = { 20, 30 };
    SDL_QueryTexture(tex, 0, 0, &r.w, &r.h);
    SDL_RenderCopy(rend, tex, 0, &r);

    SDL_DestroyTexture(tex);
}


void player_animate_weapon(struct Player *p)
{
    if (p->weapon == p->gun)
    {
        if (p->scoped)
            p->gun->pos = (Vec3f){ 0.f, .4f, .6f };
        else
            p->gun->pos = (Vec3f){ .35f, .5f, .8f };
    }
    else if (p->weapon == p->knife)
    {
        if (p->knife_thrown && vec_len(vec_sub(p->knife->mesh->pos, p->knife->pos)) <= 5.f)
        {
            p->knife->absolute = false;
            p->knife_thrown = false;
            p->knife->divisor = 5.f;
            p->knife->pos = p->knife->default_pos;
            p->knife->mesh->pos = vec_sub(vec_addv(p->cam->pos, render_rotate_cc(p->knife->pos, p->cam->angle)), vec_mulf(render_rotate_cc((Vec3f){ 0.f, 0.f, 1.f }, p->cam->angle), 2.f));
        }
    }
}


void player_hurt(struct Player *p, int damage)
{
    if ((clock() - p->last_hurt) / CLOCKS_PER_SEC > 1)
    {
        audio_play_sound("res/sfx/player_damage.wav");
        p->health -= damage;
        p->last_hurt = clock();
    }
}


void player_switch_weapon(struct Player *p, struct Weapon *weapon)
{
    if (p->weapon == weapon)
        return;

    p->scoped = false;
    p->knife_thrown = false;
    p->weapon->absolute = false;

    weapon->mesh->pos = vec_addv(p->cam->pos, render_rotate_cc((Vec3f){ .5f, -.5f, 0.f }, p->cam->angle));
    weapon->mesh->rot = vec_addv(p->cam->angle, (Vec3f){ 0.f, 1.f, 0.f });
    p->weapon->pos = p->weapon->default_pos;
    p->weapon = weapon;

    if (weapon == p->gun) audio_play_sound("res/sfx/gun_cock.wav");
    if (weapon == p->knife) audio_play_sound("res/sfx/knife_equip.wav");
}

