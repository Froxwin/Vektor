#include "primitives.h"

Polyline *mk_polyline(void) {
  Polyline *pl = malloc(sizeof(Polyline));
  pl->count = 0;
  pl->capacity = 4;
  pl->points = malloc(sizeof(V2) * pl->capacity);
  return pl;
}

void add_point_polyline(Polyline *pl, V2 point) {
  if (pl->count >= pl->capacity) {
    pl->capacity *= 2;
    pl->points = realloc(pl->points, sizeof(V2) * pl->capacity);
  }
  pl->points[pl->count++] = point;
}

void free_polyline(Polyline *pl) {
  if (!pl)
    return;
  free(pl->points);
  free(pl);
}

Polygon *mk_polygon(void) {
  Polygon *pg = malloc(sizeof(Polygon));
  pg->count = 0;
  pg->capacity = 4;
  pg->points = malloc(sizeof(V2) * pg->capacity);
  return pg;
}

void add_point_polygon(Polygon *pg, V2 point) {
  if (pg->count >= pg->capacity) {
    pg->capacity *= 2;
    pg->points = realloc(pg->points, sizeof(V2) * pg->capacity);
  }
  pg->points[pg->count++] = point;
}

void free_polygon(Polygon *pg) {
  if (!pg)
    return;
  free(pg->points);
  free(pg);
}
