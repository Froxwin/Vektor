#include "raster.h"
#include "primitives.h"
#include "stddef.h"

void vektor_edgebuffer_add_edge(EdgeBuffer *buffer, Edge edge) {
  if (buffer->count >= buffer->capacity) {
    buffer->capacity = buffer->capacity ? buffer->capacity * 2 : 4;
    buffer->edges = realloc(buffer->edges, sizeof(Edge) * buffer->capacity);
  }
  buffer->edges[buffer->count++] = edge;
}

void vektor_edgebuffer_flatten_line(EdgeBuffer *buffer, VektorLine line) {
  vektor_edgebuffer_add_edge(buffer, (Edge){line.p1, line.p2, 0});
}

void vektor_edgebuffer_flatten_polyline(EdgeBuffer *buffer, VektorPolyline *line) {
  for (size_t i = 0; i + 1 < line->count; i++) {
    vektor_edgebuffer_add_edge(buffer, (Edge){line->points[i], line->points[i + 1], 0});
  }
}

void vektor_edgebuffer_flatten_polygon(EdgeBuffer *buffer, VektorPolygon *pg) {
  size_t n = pg->count;
  if (n < 3)
    return;

  for (size_t i = 0; i < n; i++) {
    V2 p1 = pg->points[i];
    V2 p2 = pg->points[(i + 1) % n];
    int winding = (p1.y < p2.y) ? +1 : -1;
    vektor_edgebuffer_add_edge(buffer, (Edge){p1, p2, winding});
  }
}

inline VektorFramebuffer vektor_framebuffer_new(unsigned int W, unsigned int H) {
  VektorFramebuffer fb = {.width = W, .height = H, .pixels = calloc(W * H * 4, 1)};
  return fb;
}

inline void vektor_framebuffer_putpixel(VektorFramebuffer *fb, int x, int y, unsigned char r,
                      unsigned char g, unsigned char b) {
  if ((unsigned)x >= fb->width || (unsigned)y >= fb->height)
    return;

  int i = (y * fb->width + x) * 4;
  fb->pixels[i + 0] = r;
  fb->pixels[i + 1] = g;
  fb->pixels[i + 2] = b;
  fb->pixels[i + 3] = 255;
}

void vektor_framebuffer_drawline(VektorFramebuffer *fb, V2 a, V2 b, unsigned char r, unsigned char g,
               unsigned char bl) {
  int x0 = (int)a.x;
  int y0 = (int)a.y;
  int x1 = (int)b.x;
  int y1 = (int)b.y;

  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  for (;;) {
    vektor_framebuffer_putpixel(fb, x0, y0, r, g, bl);
    if (x0 == x1 && y0 == y1)
      break;

    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void vektor_framebuffer_drawto(VektorFramebuffer* fb, VektorCanvas* target) {
  for(int x = 0; x < fb->width; x++) {
    for(int y = 0; y < fb->height; y++) {

      int i = (y * fb->width + x) * 4;
      target->canvasPixels[i+0] = (guchar)fb->pixels[i+0];
      target->canvasPixels[i+1] = (guchar)fb->pixels[i+1];
      target->canvasPixels[i+2] = (guchar)fb->pixels[i+2];
      target->canvasPixels[i+3] = (guchar)fb->pixels[i+3];
    }
  }
}