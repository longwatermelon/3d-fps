#ifndef MESH_H
#define MESH_H

#include "util.h"
#include "camera.h"
#include <SDL2/SDL.h>
#include <sys/types.h>

typedef struct
{
    int idx[3];
    int nidx;
} Triangle;

struct Mesh
{
    Vec3f pos;

    Vec3f *pts;
    size_t npts;

    Triangle *tris;
    size_t ntris;
    
    Vec3f *norms;
    size_t nnorms;
};

struct Mesh *mesh_alloc(Vec3f pos, const char *fp);
void mesh_free(struct Mesh *m);

void mesh_read(struct Mesh *m, const char *fp);

void mesh_render(struct Mesh *m, SDL_Renderer *rend, struct Camera *c);

#endif

