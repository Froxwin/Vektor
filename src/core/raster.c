#include "raster.h"
#include "epoxy/gl.h"
#include "primitives.h"
#include "src/core/matrix.h"
#include "src/core/vector.h"
#include "stddef.h"
#include <math.h>
#include <stddef.h>

#define PI 3.14159265358979323846

void vektor_edgebuffer_add_edge(EdgeBuffer* buffer, Edge edge) {
    if (buffer->count >= buffer->capacity) {
        buffer->capacity = buffer->capacity ? buffer->capacity * 2 : 4;
        buffer->edges = realloc(buffer->edges, sizeof(Edge) * buffer->capacity);
    }
    buffer->edges[buffer->count++] = edge;
}

void vektor_polyline_tessellate(EdgeBuffer* buffer, VektorPolyline* line,
                                size_t j, double scale) {
    for (size_t i = 0; i + 1 < line->count; i++) {
        vektor_edgebuffer_add_edge(
            buffer, (Edge){line->points[i], line->points[i + 1], 0, j});
    }
}

void vektor_polygon_tessellate(EdgeBuffer* buffer, VektorPolygon* polygon,
                               size_t j, double scale) {
    for (size_t i = 0; i + 1 < polygon->count; i++) {
        vektor_edgebuffer_add_edge(
            buffer, (Edge){polygon->points[i], polygon->points[i + 1], 0, j});
    }
    vektor_edgebuffer_add_edge(
        buffer,
        (Edge){polygon->points[polygon->count - 1], polygon->points[0], 0, j});
}

void vektor_circle_tessellate(EdgeBuffer* buffer, VektorCircle* circle,
                              size_t j, double scale) {
    double err = 0.000025;
    size_t res = PI * sqrt((scale * circle->radius) / (2 * err));
    for (size_t i = 0; i < res; i++) {
        double theta1 = (2 * PI * i) / res;
        double theta2 = (2 * PI * (i + 1)) / res;
        V2 p1 = (V2){circle->center.x + circle->radius * cos(theta1),
                     circle->center.y + circle->radius * sin(theta1)};
        V2 p2 = (V2){circle->center.x + circle->radius * cos(theta2),
                     circle->center.y + circle->radius * sin(theta2)};
        vektor_edgebuffer_add_edge(buffer, (Edge){p1, p2, 0, j});
    }
}

void vektor_rectangle_tessellate(EdgeBuffer* buffer, VektorRectangle* rct,
                                 size_t j, double scale) {
    if (vec2_equals(rct->end, rct->start)) {
        return;
    }

    Edge top = (Edge){rct->start, (V2){rct->end.x, rct->start.y}, 0, j};
    Edge right = (Edge){(V2){rct->end.x, rct->start.y}, rct->end, 0, j};
    Edge bottom = (Edge){(V2){rct->start.x, rct->end.y}, rct->end, 0, j};
    Edge left = (Edge){rct->start, (V2){rct->start.x, rct->end.y}, 0, j};

    vektor_edgebuffer_add_edge(buffer, top);
    vektor_edgebuffer_add_edge(buffer, right);
    vektor_edgebuffer_add_edge(buffer, bottom);
    vektor_edgebuffer_add_edge(buffer, left);
}

void vektor_rasterize(VertexBuffer* vb, VektorShapeBuffer* shapes,
                      double scale) {
    EdgeBuffer edges = {0};
    for (size_t i = 0; i < shapes->count; i++) {
        VektorPrimitive* p = &shapes->shapes[i].primitive;

        switch (p->kind) {
        case VEKTOR_POLYLINE:
            vektor_polyline_tessellate(&edges, p->polyline, i, scale);
            break;

        case VEKTOR_POLYGON:
            vektor_polygon_tessellate(&edges, p->polygon, i, scale);
            break;

        case VEKTOR_CIRCLE:
            vektor_circle_tessellate(&edges, &p->circle, i, scale);
            break;

        case VEKTOR_RECTANGLE:
            vektor_rectangle_tessellate(&edges, &p->rectangle, i, scale);
            break;

        default:
            // TODO: fill in all primitives
            break;
        }
    }

    vektor_edges_to_triangles(vb, &edges, shapes);
}

void vektor_vb_add_triangle(VertexBuffer* vb, V2 v0, V2 v1, V2 v2,
                            VektorColor color) {
    if (vb->count + 3 >= vb->capacity) {
        vb->capacity = vb->capacity ? vb->capacity * 2 : 8;
        vb->vertices = realloc(vb->vertices, sizeof(Vertex) * vb->capacity);
    }
    vb->vertices[vb->count++] = (Vertex){v0, color};
    vb->vertices[vb->count++] = (Vertex){v1, color};
    vb->vertices[vb->count++] = (Vertex){v2, color};

    if (vb->count <= vb->capacity / 4) {
        vb->capacity /= 2;
        vb->vertices = realloc(vb->vertices, sizeof(Vertex) * vb->capacity);
    }
}

void vektor_vb_add_quad(VertexBuffer* vb, V2 a, V2 b, VektorColor color) {

    float minx = fminf(a.x, b.x);
    float maxx = fmaxf(a.x, b.x);
    float miny = fminf(a.y, b.y);
    float maxy = fmaxf(a.y, b.y);

    V2 tl = {minx, miny};
    V2 bl = {minx, maxy};
    V2 br = {maxx, maxy};
    V2 tr = {maxx, miny};

    vektor_vb_add_triangle(vb, tl, bl, br, color);
    vektor_vb_add_triangle(vb, tl, br, tr, color);
}

void vektor_edge_to_triangles(VertexBuffer* vb, Edge e,
                              VektorShapeBuffer* shape_buffer) {
    float dx = e.p2.x - e.p1.x;
    float dy = e.p2.y - e.p1.y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len == 0)
        return;

    float px =
        -dy / len * (shape_buffer->shapes[e.shape_id].style.stroke_width / 2);
    float py =
        dx / len * (shape_buffer->shapes[e.shape_id].style.stroke_width / 2);

    V2 v0 = m33_transform(shape_buffer->shapes[e.shape_id].transform,
                          (V2){e.p1.x + px, e.p1.y + py});
    V2 v1 = m33_transform(shape_buffer->shapes[e.shape_id].transform,
                          (V2){e.p1.x - px, e.p1.y - py});
    V2 v2 = m33_transform(shape_buffer->shapes[e.shape_id].transform,
                          (V2){e.p2.x + px, e.p2.y + py});
    V2 v3 = m33_transform(shape_buffer->shapes[e.shape_id].transform,
                          (V2){e.p2.x - px, e.p2.y - py});

    vektor_vb_add_triangle(vb, v0, v1, v2,
                           shape_buffer->shapes[e.shape_id].style.stroke_color);
    vektor_vb_add_triangle(vb, v2, v1, v3,
                           shape_buffer->shapes[e.shape_id].style.stroke_color);
}

void vektor_edges_to_triangles(VertexBuffer* vb, EdgeBuffer* edges,
                               VektorShapeBuffer* shape_buffer) {
    for (size_t i = 0; i < edges->count; i++) {
        vektor_edge_to_triangles(vb, edges->edges[i], shape_buffer);
    }
}