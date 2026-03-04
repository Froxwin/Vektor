#include "matrix.h"
#include <math.h>

inline M33 m33_zero(void) { return (M33){{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}}; }

inline M33 m33_identity(void) {
  return (M33){{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
}

inline M33 m33_translate(double tx, double ty) {
  return (M33){{{1, 0, tx}, {0, 1, ty}, {0, 0, 1}}};
}

inline M33 m33_scale(double sx, double sy) {
  return (M33){{{sx, 0, 0}, {0, sy, 0}, {0, 0, 1}}};
}

inline M33 m33_rotate(double theta) {
  return (M33){
      {{cos(theta), -sin(theta), 0}, {sin(theta), cos(theta), 0}, {0, 0, 1}}};
}

inline M33 m33_shear(double theta_x, double theta_y) {
  return (M33){{{1, tan(theta_x), 0}, {tan(theta_y), 1, 0}, {0, 0, 0}}};
}

M33 m33_add(const M33 m1, const M33 m2) {
  M33 res;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      res.m[i][j] = m1.m[i][j] + m2.m[i][j];
  return res;
}

M33 m33_sub(const M33 m1, const M33 m2) {
  M33 res;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      res.m[i][j] = m1.m[i][j] - m2.m[i][j];
  return res;
}

M33 m33_mul(const M33 m1, const M33 m2) {
  M33 res = {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
        res.m[i][j] += m1.m[i][k] * m2.m[k][j];
  return res;
}

M33 m33_transpose(const M33 m) {
  M33 res;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      res.m[i][j] = m.m[j][i];
  return res;
}

// 2D affine transform inversion (not general 3x3 inversion)
M33 m33_inverse(const M33 m) {
  M33 inv;
  double det;

  det = m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0];
  if (det == 0.0) {
    return m33_identity();
  }
  double invDet = 1.0 / det;

  inv.m[0][0] = m.m[1][1] * invDet;
  inv.m[0][1] = -m.m[0][1] * invDet;
  inv.m[1][0] = -m.m[1][0] * invDet;
  inv.m[1][1] = m.m[0][0] * invDet;

  inv.m[0][2] = -(inv.m[0][0] * m.m[0][2] + inv.m[0][1] * m.m[1][2]);
  inv.m[1][2] = -(inv.m[1][0] * m.m[0][2] + inv.m[1][1] * m.m[1][2]);

  inv.m[2][0] = 0;
  inv.m[2][1] = 0;
  inv.m[2][2] = 1;

  return inv;
}

V2 m33_transform(const M33 mat, const V2 v) {
  return (V2){mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2],
              mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2]};
}
