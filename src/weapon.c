#include "weapon.h"
#include "render.h"


struct Weapon *weapon_alloc(const char *fp, Vec3f pos, Vec3f angle)
{
    struct Weapon *w = malloc(sizeof(struct Weapon));
    w->mesh = mesh_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f }, fp, (SDL_Color){ 255, 255, 255 });
    w->pos = pos;
    w->angle = angle;

    return w;
}


void weapon_free(struct Weapon *w)
{
    mesh_free(w->mesh);
    free(w);
}


void weapon_move(struct Weapon *w, struct Camera *c, float divisor)
{
    Vec3f pos = vec_addv(c->pos, render_rotate_cc(w->pos, c->angle));
    w->mesh->pos = vec_addv(w->mesh->pos, vec_divf(vec_sub(pos, w->mesh->pos), divisor));

    w->mesh->rot = vec_addv(w->mesh->rot, vec_divf(vec_sub(vec_addv(c->angle, w->angle), w->mesh->rot), divisor));
}


void weapon_render(struct Weapon *w, SDL_Renderer *rend, struct Camera *c)
{
    mesh_render(w->mesh, rend, c);
}

