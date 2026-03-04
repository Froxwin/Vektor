#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "stddef.h"
#include "stdlib.h"
#include "vector.h"

typedef struct {
  V2 p1;
  V2 p2;
} Line;

typedef struct {
  V2 *points;
  size_t count;
  size_t capacity;
} Polyline;

typedef struct {
  V2 *points;
  size_t count;
  size_t capacity;
} Polygon;

typedef struct {
  V2 center;
  double radius;
} Circle;

typedef enum { LINE, POLYLINE, POLYGON, CIRCLE } PrimitiveKind;

typedef struct {
  PrimitiveKind kind;
  union {
    Line line;
    Polyline *polyline;
    Polygon *polygon;
    Circle circle;
  };
} Primitive;

Polyline *mk_polyline(void);
void add_point_polyline(Polyline *pl, V2 point);
void free_polyline(Polyline *pl);

Polygon *mk_polygon(void);
void add_point_polygon(Polygon *pl, V2 point);
void free_polygon(Polygon *pl);

#endif // PRIMITIVES_H_
