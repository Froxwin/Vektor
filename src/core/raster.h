#ifndef RASTER_H_
#define RASTER_H_

#include "primitives.h"

#include "../util/color.h"
#include "stddef.h"
#include "vector.h"
#include <stddef.h>

typedef struct {
    V2 p1;
    V2 p2;
    int winding;
    size_t shape_id;
} Edge;

typedef struct {
    Edge* edges;
    size_t count;
    size_t capacity;
} EdgeBuffer;

void vektor_edgebuffer_add_edge(EdgeBuffer* edges, Edge edge);

void vektor_polyline_tessellate(EdgeBuffer* edges, VektorPolyline* line, size_t i);
void vektor_polygon_tessellate(EdgeBuffer* buffer, VektorPolygon* polygon, size_t i);
void vektor_rectangle_tessellate(EdgeBuffer* buffer, VektorRectangle* rct, size_t i);

typedef struct {
    V2 coords;
    VektorColor color;
} Vertex;

typedef struct {
    Vertex* vertices;
    size_t count;
    size_t capacity;
} VertexBuffer;

void vb_add_triangle(VertexBuffer* vb, V2 v0, V2 v1, V2 v2, VektorColor color);
void vektor_edge_to_triangles(VertexBuffer* vb, Edge e,
                              VektorShapeBuffer* shape_buffer);
void vektor_edges_to_triangles(VertexBuffer* vb, EdgeBuffer* edges,
                               VektorShapeBuffer* shape_buffer);
void vektor_rasterize(VertexBuffer* vb, VektorShapeBuffer* shapes);

#endif // RASTER_H_
