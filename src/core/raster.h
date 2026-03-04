#ifndef RASTER_H_
#define RASTER_H_

#include "primitives.h"
#include "stddef.h"
#include "vector.h"

typedef struct {
  V2 p1;
  V2 p2;
  int winding;
} Edge;

typedef struct {
  Edge *edges;
  size_t count;
  size_t capacity;
} EdgeBuffer;

void add_edge(EdgeBuffer *edges, Edge edge);

void flatten_line(EdgeBuffer *edges, Line line);
void flatten_polyline(EdgeBuffer *edges, Polyline *line);
void flatten_polygon(EdgeBuffer *buffer, Polygon *line);

typedef struct {
  unsigned int width;
  unsigned int height;
  unsigned char *pixels; // Flat RGB8 array
} Framebuffer;

Framebuffer mk_framebuffer(unsigned int width, unsigned int height);

void put_pixel(Framebuffer *fb, int x, int y, unsigned char r, unsigned char g,
               unsigned char b);

void draw_line(Framebuffer *fb, V2 a, V2 b, unsigned char r, unsigned char g,
               unsigned char bl);

#endif // RASTER_H_
