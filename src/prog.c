#include "prog.h"
#include "mesh.h"


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;

    p->window = w;
    p->rend = r;

    p->cam = cam_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f });

    return p;
}


void prog_free(struct Prog *p)
{
    cam_free(p->cam);
    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    struct Mesh *m = mesh_alloc((Vec3f){ 0.f, 0.f, 5.f }, "res/donut.obj");
    struct Mesh *m2 = mesh_alloc((Vec3f){ 5.f, 0.f, 0.f }, "res/donut.obj");
    struct Mesh *m3 = mesh_alloc((Vec3f){ 0.f, 0.f, -5.f }, "res/donut.obj");
    struct Mesh *m4 = mesh_alloc((Vec3f){ -5.f, 0.f, 0.f }, "res/donut.obj");

    while (p->running)
    {
        prog_events(p, &evt);

        SDL_RenderClear(p->rend);

        mesh_render(m, p->rend, p->cam);
        mesh_render(m2, p->rend, p->cam);
        mesh_render(m3, p->rend, p->cam);
        mesh_render(m4, p->rend, p->cam);

        p->cam->angle.x += .03f;
//        p->cam->angle.y += .03f;
 //       p->cam->angle.z += .03f;

        SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 255);
        SDL_RenderPresent(p->rend);
    }

    mesh_free(m);
    mesh_free(m2);
    mesh_free(m3);
    mesh_free(m4);
}


void prog_events(struct Prog *p, SDL_Event *evt)
{
    while (SDL_PollEvent(evt))
    {
        switch (evt->type)
        {
        case SDL_QUIT:
            p->running = false;
            break;
        }
    }
}

