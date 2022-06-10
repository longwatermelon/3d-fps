#include "render.h"

#define swapp(x, y) { \
    SDL_Point tmp = x; \
    x = y; \
    y = tmp; }

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


void render_filled_tri(SDL_Point p[3], uint32_t *screen, SDL_Color col)
{
    SDL_Point p0 = p[0], p1 = p[1], p2 = p[2];
    if (p0.y > p1.y) swapp(p0, p1);
    if (p0.y > p2.y) swapp(p0, p2);
    if (p1.y > p2.y) swapp(p1, p2);

    float s10 = (float)(p1.y - p0.y) / (p1.x - p0.x);
    float s20 = (float)(p2.y - p0.y) / (p2.x - p0.x);
    float s21 = (float)(p2.y - p1.y) / (p2.x - p1.x);

    float x1 = render_fill_edges(p0.x, p0.x, p0.y, p1.y, s20, s10, screen, col);
    render_fill_edges(x1, p1.x, p1.y, p2.y, s20, s21, screen, col);
}


float render_fill_edges(float x1, float x2, int top, int bot, float s1, float s2, uint32_t *screen, SDL_Color col)
{
    for (int y = top; y < bot; ++y)
    {
        /* x1 = fmin(fmax(x1, 0.f), 800.f); */
        /* x2 = fmin(fmax(x2, 0.f), 800.f); */
        float min = x1 > x2 ? x2 : x1;
        float max = x1 > x2 ? x1 : x2;

        for (float i = roundf(min); i < roundf(max); ++i)
        {
            if (i < 0)
            {
                i = 0;
                continue;
            }
            if (i > 800) break;

            int idx = y * 800 + (int)i;

            if (idx >= 0 && idx < 800 * 800)
                screen[y * 800 + (int)i] = 0x00000000 | col.r << 16 | col.g << 8 | col.b;
        }

        x1 += 1.f / s1;
        x2 += 1.f / s2;
    }

    return x1;
}

