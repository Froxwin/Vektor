#ifndef VECTOR_H_
#define VECTOR_H_

#include "math.h"

typedef struct {
    float x;
    float y;
} V2;

typedef struct {
    double x;
    double y;
    double z;
} V3;

static inline V3 vec2_vector(const V2 v) { return (V3){v.x, v.y, 0}; }

static inline V3 vec2_point(const V2 v) { return (V3){v.x, v.y, 1}; }

static inline V2 vec2_add(const V2 v1, const V2 v2) {
    return (V2){v1.x + v2.x, v1.y + v2.y};
}

static inline V2 vec2_sub(const V2 v1, const V2 v2) {
    return (V2){v1.x - v2.x, v1.y - v2.y};
}

static inline V2 vec2_mul(const V2 v1, const V2 v2) {
    return (V2){v1.x * v2.x, v1.y * v2.y};
}

static inline V2 vec2_scale(const V2 v, const double k) {
    return (V2){v.x * k, v.y * k};
}

static inline double vec2_dot(const V2 v1, const V2 v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

static inline double vec2_cross(const V2 a, const V2 b) {
    return a.x * b.y - a.y * b.x;
}

static inline double vec2_norm(const V2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

static inline double vec2_quadrance(const V2 v) {
    return (v.x * v.x + v.y * v.y);
}

static inline V2 vec2_normalize(const V2 v) {
    return vec2_scale(v, 1 / vec2_norm(v));
}

#endif // VECTOR_H_
