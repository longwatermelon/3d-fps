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

