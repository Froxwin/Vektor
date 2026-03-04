#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "stddef.h"
#include "stdlib.h"
#include "vector.h"

typedef struct {
  V2 p1;
  V2 p2;
} VektorLine;

typedef struct {
  V2 *points;
  size_t count;
  size_t capacity;
} VektorPolyline;

typedef struct {
  V2 *points;
  size_t count;
  size_t capacity;
} VektorPolygon;

typedef struct {
  V2 center;
  double radius;
} VektorCircle;

typedef enum { LINE, POLYLINE, POLYGON, CIRCLE } VektorPrimitiveKind;

typedef struct {
  VektorPrimitiveKind kind;
  union {
    VektorLine line;
    VektorPolyline *polyline;
    VektorPolygon *polygon;
    VektorCircle circle;
  };
} VektorPrimitive;

VektorPolyline* vektor_polyline_new(void);
void vektor_polyline_add_point(VektorPolyline *pl, V2 point);
void vektor_polyline_free(VektorPolyline *pl);

VektorPolygon *vektor_polygon_new(void);
void vektor_polygon_add_point(VektorPolygon *pl, V2 point);
void vektor_polygon_free(VektorPolygon *pl);

#endif // PRIMITIVES_H_
