#include "weapon.h"
#include "render.h"


struct Weapon *weapon_alloc(const char *fp, Vec3f pos, Vec3f angle, float divisor)
{
    struct Weapon *w = malloc(sizeof(struct Weapon));
    w->mesh = mesh_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f }, fp, (SDL_Color){ 112, 181, 255 });
    w->default_pos = pos;
    w->pos = pos;
    w->angle = angle;

    w->divisor = divisor;

    return w;
}


void weapon_free(struct Weapon *w)
{
    mesh_free(w->mesh);
    free(w);
}


void weapon_move(struct Weapon *w, struct Camera *c, bool absolute)
{
    Vec3f pos;

    if (absolute)
        pos = w->pos;
    else
        pos = vec_addv(c->pos, render_rotate_cc(w->pos, c->angle));

    w->mesh->pos = vec_addv(w->mesh->pos, vec_divf(vec_sub(pos, w->mesh->pos), w->divisor));

    if (!absolute)
        w->mesh->rot = vec_addv(w->mesh->rot, vec_divf(vec_sub(vec_addv(c->angle, w->angle), w->mesh->rot), w->divisor));
}


void weapon_render(struct Weapon *w, SDL_Renderer *rend, struct Camera *c)
{
    mesh_render(w->mesh, rend, c);
}

