#include "render.h"

#define swapp(x, y) { \
    SDL_Point tmp = x; \
    x = y; \
    y = tmp; }

#define swapf(x, y) { \
    float tmp = x; \
    x = y; \
    y = tmp; }

RTI *rti_alloc(float x, float z, float sx, float sz)
{
    RTI *rti = malloc(sizeof(RTI));
    rti->x = x;
    rti->z = z;
    rti->sx = sx;
    rti->sz = sz;

    return rti;
}

void rti_free(RTI *rti)
{
    free(rti);
}

SDL_Point render_project_point(Vec3f p)
{
    float px = 0.f, py = 0.f;

    if (p.z != 0.f)
    {
        px = p.x / p.z;
        py = p.y / p.z;
    }

    SDL_Point proj = {
        (px + .5f) * 800.f,
        (py + .5f) * 800.f
    };

    return proj;
}


Vec3f render_rotate_ccw(Vec3f p, Vec3f angle)
{
    float rotx[3][3] = {
        { 1, 0, 0 },
        { 0, cosf(angle.y), -sinf(angle.y) },
        { 0, sinf(angle.y), cosf(angle.y) }
    };

    float roty[3][3] = {
        { cosf(angle.x), 0, sinf(angle.x) },
        { 0, 1, 0 },
        { -sinf(angle.x), 0, cosf(angle.x) }
    };

    float rotz[3][3] = {
        { cosf(angle.z), -sinf(angle.z), 0 },
        { sinf(angle.z), cosf(angle.z), 0 },
        { 0, 0, 1 }
    };

    return util_matmul(rotz, util_matmul(rotx, util_matmul(roty, p)));
}


Vec3f render_rotate_cc(Vec3f p, Vec3f angle)
{
    angle = vec_mulf(angle, -1.f);

    float rotx[3][3] = {
        { 1, 0, 0 },
        { 0, cosf(angle.y), -sinf(angle.y) },
        { 0, sinf(angle.y), cosf(angle.y) }
    };

    float roty[3][3] = {
        { cosf(angle.x), 0, sinf(angle.x) },
        { 0, 1, 0 },
        { -sinf(angle.x), 0, cosf(angle.x) }
    };

    float rotz[3][3] = {
        { cosf(angle.z), -sinf(angle.z), 0 },
        { sinf(angle.z), cosf(angle.z), 0 },
        { 0, 0, 1 }
    };

    return util_matmul(roty, util_matmul(rotx, util_matmul(rotz, p)));
}


SDL_Texture *render_text(SDL_Renderer *rend, TTF_Font *font, const char *s)
{
    if (strlen(s) == 0)
        return 0;

    SDL_Surface *surf = TTF_RenderText_Blended(font, s, (SDL_Color){ 255, 255, 255 });
    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);
    return tex;
}


void render_filled_tri(SDL_Point p[3], float z[3], uint32_t *screen, SDL_Color col)
{
    SDL_Point p0 = p[0], p1 = p[1], p2 = p[2];
    float z0 = z[0], z1 = z[1], z2 = z[2];

    if (p0.y > p1.y)
    {
        swapp(p0, p1);
        swapf(z0, z1);
    }

    if (p0.y > p2.y)
    {
        swapp(p0, p2);
        swapf(z0, z2);
    }

    if (p1.y > p2.y)
    {
        swapp(p1, p2);
        swapf(z1, z2);
    }

    float s10 = (float)(p1.y - p0.y) / (p1.x - p0.x);
    float s20 = (float)(p2.y - p0.y) / (p2.x - p0.x);
    float s21 = (float)(p2.y - p1.y) / (p2.x - p1.x);

    RTI *r02 = rti_alloc(p0.x, z0, s20, (float)(z2 - z0) / (p2.y - p0.y));
    RTI *r01 = rti_alloc(p0.x, z0, s10, (float)(z1 - z0) / (p1.y - p0.y));
    RTI *r12 = rti_alloc(p1.x, z1, s21, (float)(z2 - z1) / (p2.y - p1.y));

    render_fill_edges(p0.y, p1.y, r02, r01, screen, col);
    render_fill_edges(p1.y, p2.y, r02, r12, screen, col);

    rti_free(r02);
    rti_free(r01);
    rti_free(r12);
}


void render_fill_edges(int top, int bot, RTI *l1, RTI *l2, uint32_t *screen, SDL_Color col)
{
    for (int y = top; y < bot; ++y)
    {
        int min = roundf(l1->x > l2->x ? l2->x : l1->x);
        int max = roundf(l1->x > l2->x ? l1->x : l2->x);

        for (int i = min; i < max; ++i)
        {
            if (i < 0)
            {
                i = 0;
                continue;
            }
            if (i > 800) break;

            int idx = y * 800 + i;

            if (idx >= 0 && idx < 800 * 800)
                screen[idx] = 0x00000000 | col.r << 16 | col.g << 8 | col.b;
        }

        l1->x += 1.f / l1->sx;
        l2->x += 1.f / l2->sx;
    }
}

