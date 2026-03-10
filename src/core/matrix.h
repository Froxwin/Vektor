#ifndef MATRIX_H_
#define MATRIX_H_

#include "vector.h"

// Row major 3x3 matricies
typedef struct {
    double m[3][3];
} M33;

M33 m33_identity(void);
M33 m33_translate(double tx, double ty);
M33 m33_scale(double sx, double sy);
M33 m33_rotate(double rad);
M33 m33_shear(double theta_x, double theta_y);

M33 m33_add(const M33 m1, const M33 m2);
M33 m33_sub(const M33 m1, const M33 m2);
M33 m33_mul(const M33 m1, const M33 m2);
M33 m33_transpose(const M33 m);
M33 m33_inverse(const M33 m);

V2 m33_transform(const M33 mat, const V2 v);

void m33_to_gl4(const M33 m, float out[16]);

#endif // MATRIX_H_
