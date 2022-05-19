#ifndef UTIL_H
#define UTIL_H

typedef struct
{
    float x, y, z;
} Vec3f;

Vec3f util_matmul(float mat[3][3], Vec3f p);

#endif

