#include "raster.h"
#include "primitives.h"
#include "stddef.h"

void add_edge(EdgeBuffer *buffer, Edge edge) {
  if (buffer->count >= buffer->capacity) {
    buffer->capacity = buffer->capacity ? buffer->capacity * 2 : 4;
    buffer->edges = realloc(buffer->edges, sizeof(Edge) * buffer->capacity);
  }
  buffer->edges[buffer->count++] = edge;
}

void flatten_line(EdgeBuffer *buffer, Line line) {
  add_edge(buffer, (Edge){line.p1, line.p2, 0});
}

void flatten_polyline(EdgeBuffer *buffer, Polyline *line) {
  for (size_t i = 0; i + 1 < line->count; i++) {
    add_edge(buffer, (Edge){line->points[i], line->points[i + 1], 0});
  }
}

void flatten_polygon(EdgeBuffer *buffer, Polygon *pg) {
  size_t n = pg->count;
  if (n < 3)
    return;

  for (size_t i = 0; i < n; i++) {
    V2 p1 = pg->points[i];
    V2 p2 = pg->points[(i + 1) % n];
    int winding = (p1.y < p2.y) ? +1 : -1;
    add_edge(buffer, (Edge){p1, p2, winding});
  }
}

inline Framebuffer mk_framebuffer(unsigned int W, unsigned int H) {
  Framebuffer fb = {.width = W, .height = H, .pixels = calloc(W * H * 3, 1)};
  return fb;
}

inline void put_pixel(Framebuffer *fb, int x, int y, unsigned char r,
                      unsigned char g, unsigned char b) {
  if ((unsigned)x >= fb->width || (unsigned)y >= fb->height)
    return;

  int i = (y * fb->width + x) * 3;
  fb->pixels[i + 0] = r;
  fb->pixels[i + 1] = g;
  fb->pixels[i + 2] = b;
}

void draw_line(Framebuffer *fb, V2 a, V2 b, unsigned char r, unsigned char g,
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
    put_pixel(fb, x0, y0, r, g, bl);
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