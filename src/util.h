#ifndef UTIL_H
#define UTIL_H

#include "vector.h"

Vec3f util_matmul(float mat[3][3], Vec3f p);

float util_tri_area(Vec3f points[3]);

Vec3f util_barycentric_coefficients(Vec3f points[3], Vec3f p);

float util_p_line_sdist(Vec3f p, Vec3f p0, Vec3f p1);

#endif

