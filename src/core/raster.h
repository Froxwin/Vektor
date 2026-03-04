#ifndef RASTER_H_
#define RASTER_H_

#include "primitives.h"
#include "../ui/vektorcanvas.h"

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

void vektor_edgebuffer_add_edge(EdgeBuffer *edges, Edge edge);

void vektor_edgebuffer_flatten_line(EdgeBuffer *edges, VektorLine line);
void vektor_edgebuffer_flatten_polyline(EdgeBuffer *edges, VektorPolyline *line);
void vektor_edgebuffer_flatten_polygon(EdgeBuffer *buffer, VektorPolygon *line);

typedef struct {
  unsigned int width;
  unsigned int height;
  unsigned char *pixels; // Flat RGBA8 array
} VektorFramebuffer;

VektorFramebuffer vektor_framebuffer_new(unsigned int width, unsigned int height);

void vektor_framebuffer_putpixel(VektorFramebuffer *fb, int x, int y, unsigned char r, unsigned char g,
               unsigned char b);

void vektor_framebuffer_drawline(VektorFramebuffer *fb, V2 a, V2 b, unsigned char r, unsigned char g,
               unsigned char bl);

void vektor_framebuffer_drawto(VektorFramebuffer* fb, VektorCanvas* canvas);

#endif // RASTER_H_
