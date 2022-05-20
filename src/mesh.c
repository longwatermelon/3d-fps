#include "mesh.h"
#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


struct Mesh *mesh_alloc(Vec3f pos, const char *fp)
{
    struct Mesh *m = malloc(sizeof(struct Mesh));
    m->pos = pos;

    m->pts = 0;
    m->npts = 0;

    m->tris = 0;
    m->ntris = 0;

    m->norms = 0;
    m->nnorms = 0;

    mesh_read(m, fp);

    return m;
}


void mesh_free(struct Mesh *m)
{
    free(m->pts);
    free(m->tris);
    free(m->norms);
    free(m);
}


void mesh_read(struct Mesh *m, const char *fp)
{
    FILE *f = fopen(fp, "r");

    if (!f)
    {
        fprintf(stderr, "Error: couldn't open file '%s'.\n", fp);
        exit(EXIT_FAILURE);
    }

    char *line = 0;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1)
    {
        if (line[0] == 'v')
        {
            if (line[1] == ' ')
            {
                m->pts = realloc(m->pts, sizeof(Vec3f) * ++m->npts);
                Vec3f *p = &m->pts[m->npts - 1];

                sscanf(line, "%*s %f %f %f", &p->x, &p->y, &p->z);
            }
            else if (line[1] == 'n')
            {
                m->norms = realloc(m->norms, sizeof(Vec3f) * ++m->nnorms);
                Vec3f *n = &m->norms[m->nnorms - 1];

                sscanf(line, "%*s %f %f %f", &n->x, &n->y, &n->z);
            }
        }
        else if (line[0] == 'f')
        {
            int ws = 0;

            for (int i = 0; i < strlen(line); ++i)
            {
                if (line[i] == ' ')
                    ++ws;
            }

            if (ws != 3)
            {
                fprintf(stderr, "Error in file '%s': faces must be triangulated.\n", fp);
                exit(EXIT_FAILURE);
            }

            m->tris = realloc(m->tris, sizeof(Triangle) * ++m->ntris);

            int idx[3];
            int norm;
            sscanf(line, "%*s %d/%*d/%d %d%*s %d%*s", idx, &norm, idx + 1, idx + 2);

            m->tris[m->ntris - 1].idx[0] = idx[0] - 1;
            m->tris[m->ntris - 1].idx[1] = idx[1] - 1;
            m->tris[m->ntris - 1].idx[2] = idx[2] - 1;

            m->tris[m->ntris - 1].nidx = norm - 1;
        }
    }

    free(line);
    fclose(f);
}


void mesh_render(struct Mesh *m, SDL_Renderer *rend, struct Camera *c)
{
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

    for (size_t i = 0; i < m->ntris; ++i)
    {
        SDL_Point points[3];
        bool render = true;

        for (int j = 0; j < 3; ++j)
        {
            Vec3f p = vec_addv(m->pts[m->tris[i].idx[j]], m->pos);
            p = vec_sub(p, c->pos);

            p = render_rotate_ccw(p, c->angle);

            if (p.z <= 1.f)
            {
                render = false;
            }
            else
            {
                points[j] = render_project_point(p);
            }
        }

        if (render)
        {
            SDL_RenderDrawLine(rend, points[0].x, points[0].y, points[1].x, points[1].y);
            SDL_RenderDrawLine(rend, points[0].x, points[0].y, points[2].x, points[2].y);
            SDL_RenderDrawLine(rend, points[1].x, points[1].y, points[2].x, points[2].y);
        }
    }
}

