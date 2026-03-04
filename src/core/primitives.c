#include "primitives.h"

VektorPolyline *vektor_polyline_new(void) {
  VektorPolyline *pl = malloc(sizeof(VektorPolyline));
  pl->count = 0;
  pl->capacity = 4;
  pl->points = malloc(sizeof(V2) * pl->capacity);
  return pl;
}

void vektor_polyline_add_point(VektorPolyline *pl, V2 point) {
  if (pl->count >= pl->capacity) {
    pl->capacity *= 2;
    pl->points = realloc(pl->points, sizeof(V2) * pl->capacity);
  }
  pl->points[pl->count++] = point;
}

void vektor_polyline_free(VektorPolyline *pl) {
  if (!pl)
    return;
  free(pl->points);
  free(pl);
}

VektorPolygon *vektor_polygon_new(void) {
  VektorPolygon *pg = malloc(sizeof(VektorPolygon));
  pg->count = 0;
  pg->capacity = 4;
  pg->points = malloc(sizeof(V2) * pg->capacity);
  return pg;
}

void vektor_polygon_add_point(VektorPolygon *pg, V2 point) {
  if (pg->count >= pg->capacity) {
    pg->capacity *= 2;
    pg->points = realloc(pg->points, sizeof(V2) * pg->capacity);
  }
  pg->points[pg->count++] = point;
}

void vektor_polygon_free(VektorPolygon *pg) {
  if (!pg)
    return;
  free(pg->points);
  free(pg);
}
