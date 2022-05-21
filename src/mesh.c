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

            if (p.z < .01f)
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


bool mesh_ray_intersect(struct Mesh *m, Vec3f ro, Vec3f rdir, float *t, Triangle *tri)
{
    float nearest = INFINITY;
    *t = INFINITY;

    for (size_t i = 0; i < m->ntris; ++i)
    {
        if (vec_dot(rdir, m->norms[m->tris[i].nidx]) > 0.f)
            continue;

        if (mesh_ray_tri_intersect(m, m->tris[i], ro, rdir, &nearest))
        {
            if (nearest < *t)
            {
                *t = nearest;
                *tri = m->tris[i];
            }
        }
    }

    return *t != INFINITY;
}


bool mesh_ray_tri_intersect(struct Mesh *m, Triangle tri, Vec3f ro, Vec3f rdir, float *t)
{
    // find intersection point
    Vec3f a = m->pts[tri.idx[0]];
    a = vec_addv(a, m->pos);

    Vec3f b = m->pts[tri.idx[1]];
    b = vec_addv(b, m->pos);

    Vec3f c = m->pts[tri.idx[2]];
    c = vec_addv(c, m->pos);

    Vec3f norm = m->norms[tri.nidx];
    *t = (vec_dot(a, norm) - vec_dot(ro, norm)) / vec_dot(rdir, norm);

    // check if inside triangle
    Vec3f p = vec_addv(ro, vec_mulf(rdir, *t));

    Vec3f ca = vec_sub(c, a);
    Vec3f ba = vec_sub(b, a);

    float a1 = vec_dot(ba, ba);
    float b1 = vec_dot(ca, ba);
    float b2 = vec_dot(ca, ca);

    float c1 = vec_dot(ba, vec_sub(p, a));
    float c2 = vec_dot(ca, vec_sub(p, a));

    float y = ((c1 * b1) - (c2 * a1)) / ((b1 * b1) - (a1 * b2));
    float x = (c1 - (b1 * y)) / a1;

    return (x >= 0.f && x <= 1.f && y >= 0.f && y <= 1.f && x + y >= 0.f && x + y <= 1.f && *t >= 0.f);
}


float mesh_point_shortest_dist(struct Mesh *m, Vec3f p)
{
#if 0
    float min = INFINITY;

    for (size_t i = 0; i < m->ntris; ++i)
    {
        if (mesh_point_shortest_dist_tri(m, m->tris[i], p, t))
        {
            Vec3f point = vec_addv(p, vec_mulf(m->norms[m->tris[i].nidx], *t));
            Vec3f dir = vec_normalize(vec_sub(point, p));
//            printf("%f %f %f\n", dir.x, dir.y, dir.z);

            float dist;
            if (mesh_ray_tri_intersect(m, m->tris[i], p, dir, &dist))
            {
                if (*t < min)
                {
                    min = *t;
                    printf("%f\n", min);
                }
            }
        }
    }

    return min != INFINITY;
#endif

    float t = INFINITY;

    for (size_t i = 0; i < m->ntris; ++i)
    {
        float dist = mesh_point_shortest_dist_tri(m, m->tris[i], p);

        if (dist < t)
            t = dist;
    }

//    printf("t = %f\n", t);
    return t;
}


float mesh_point_shortest_dist_tri(struct Mesh *m, Triangle tri, Vec3f p)
{
    float t = INFINITY;

    Vec3f a = vec_addv(m->pos, m->pts[tri.idx[0]]);
    Vec3f b = vec_addv(m->pos, m->pts[tri.idx[1]]);
    Vec3f c = vec_addv(m->pos, m->pts[tri.idx[2]]);

    t = fabsf(vec_dot(vec_sub(a, p), m->norms[tri.nidx]));

    Vec3f points[3] = { a, b, c };

    Vec3f coefficients = util_barycentric_coefficients(points, vec_addv(p, vec_mulf(m->norms[tri.nidx], t)));

    int negatives = 0;
    if (coefficients.x < 0.f) ++negatives;
    if (coefficients.y < 0.f) ++negatives;
    if (coefficients.z < 0.f) ++negatives;

    if (negatives == 0)
    {
        return t;
    }
    else if (negatives == 1)
    {
        if (coefficients.x < 0.f) return util_p_line_sdist(p, points[1], points[2]);
        if (coefficients.y < 0.f) return util_p_line_sdist(p, points[0], points[2]);
        if (coefficients.z < 0.f) return util_p_line_sdist(p, points[0], points[1]);
    }
    else
    {
        if (coefficients.x >= 0.f) return vec_len(vec_sub(points[0], p));
        if (coefficients.y >= 0.f) return vec_len(vec_sub(points[1], p));
        if (coefficients.z >= 0.f) return vec_len(vec_sub(points[2], p));
    }

    return t;
}

