#include "render.h"


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

