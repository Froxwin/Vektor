#include "raster.h"
#include "primitives.h"
#include "stddef.h"
#include <stddef.h>

void vektor_edgebuffer_add_edge(EdgeBuffer *buffer, Edge edge) {
    if (buffer->count >= buffer->capacity) {
        buffer->capacity = buffer->capacity ? buffer->capacity * 2 : 4;
        buffer->edges = realloc(buffer->edges, sizeof(Edge) * buffer->capacity);
    }
    buffer->edges[buffer->count++] = edge;
}

void vektor_line_flatten(EdgeBuffer *buffer, VektorLine line) {
    vektor_edgebuffer_add_edge(buffer, (Edge){line.p1, line.p2, 0});
}

void vektor_polyline_flatten(EdgeBuffer *buffer, VektorPolyline *line) {
    for (size_t i = 0; i + 1 < line->count; i++) {
        vektor_edgebuffer_add_edge(
            buffer, (Edge){line->points[i], line->points[i + 1], 0});
    }
}

void vektor_polygon_flatten(EdgeBuffer *buffer, VektorPolygon *pg) {
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

inline VektorFramebuffer vektor_framebuffer_new(unsigned int W,
                                                unsigned int H) {
    VektorFramebuffer fb = {
        .width = W, .height = H, .pixels = calloc(W * H * 4, 1)};
    return fb;
}

inline void vektor_framebuffer_putpixel(VektorFramebuffer *fb, int x, int y,
                                        VektorColor color) {
    if ((unsigned)x >= fb->width || (unsigned)y >= fb->height)
        return;

    int i = (y * fb->width + x) * 4;
    fb->pixels[i + 0] = color.r;
    fb->pixels[i + 1] = color.g;
    fb->pixels[i + 2] = color.b;
    fb->pixels[i + 3] = color.a;
}

void draw_filled_circle(VektorFramebuffer *fb, int cx, int cy, int r,
                        VektorColor color) {
    for (int y = -r; y <= r; y++) {
        int dx = (int)sqrt(r * r - y * y);
        for (int x = -dx; x <= dx; x++) {
            vektor_framebuffer_putpixel(fb, cx + x, cy + y, color);
        }
    }
}

void vektor_framebuffer_drawline(VektorFramebuffer *fb, V2 a, V2 b,
                                 VektorColor color, double thickness) {
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
        draw_filled_circle(fb, x0, y0, thickness / 2, color);
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

void vektor_framebuffer_rasterize(VektorFramebuffer *fb,
                                  VektorPrimitiveBuffer *prims) {
    EdgeBuffer edges = {0};
    for (size_t i = 0; i < prims->count; i++) {
        VektorPrimitive *p = &prims->primitives[i];

        switch (p->kind) {
        case VEKTOR_LINE:
            vektor_line_flatten(&edges, p->line);
            break;

        case VEKTOR_POLYLINE:
            vektor_polyline_flatten(&edges, p->polyline);
            break;

        case VEKTOR_POLYGON:
            vektor_polygon_flatten(&edges, p->polygon);
            break;

        default:
            // TODO fill in all primitives
            break;
        }
    }

    for (size_t i = 0; i < edges.count; i++) {
        vektor_framebuffer_drawline(fb, edges.edges[i].p1, edges.edges[i].p2,
                                    vektor_color_solid(255, 0, 255), 4);
    }
}