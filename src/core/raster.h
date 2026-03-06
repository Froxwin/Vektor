#ifndef RASTER_H_
#define RASTER_H_

#include "primitives.h"

#include "src/util/color.h"
#include "stddef.h"
#include "vector.h"

typedef struct {
    V2 p1;
    V2 p2;
    int winding;
} Edge;

typedef struct {
    Edge* edges;
    size_t count;
    size_t capacity;
} EdgeBuffer;

void vektor_edgebuffer_add_edge(EdgeBuffer* edges, Edge edge);

void vektor_line_flatten(EdgeBuffer* edges, VektorLine line);
void vektor_polyline_flatten(EdgeBuffer* edges, VektorPolyline* line);
void vektor_polygon_flatten(EdgeBuffer* buffer, VektorPolygon* line);

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned char* pixels; // Flat RGBA8 array
} VektorFramebuffer;

VektorFramebuffer vektor_framebuffer_new(unsigned int width,
                                         unsigned int height);

void vektor_framebuffer_putpixel(VektorFramebuffer* fb, int x, int y,
                                 VektorColor color);

void vektor_framebuffer_drawline(VektorFramebuffer* fb, V2 a, V2 b,
                                 VektorColor color, double thickness);

void vektor_framebuffer_rasterize(VektorFramebuffer* fb,
                                  VektorPrimitiveBuffer* primitives);

typedef struct {
    V2* vertices;
    size_t count;
    size_t capacity;
} VertexBuffer;

void vb_add_triangle(VertexBuffer* vb, V2 v0, V2 v1, V2 v2);
void vektor_edge_to_triangles(VertexBuffer* vb, Edge e, float thickness);
VertexBuffer vektor_edges_to_triangles(EdgeBuffer* edges, float thickness);
VertexBuffer vektor_rasterize(VektorPrimitiveBuffer* prims);

#endif // RASTER_H_
