#include "raster.h"
#include "epoxy/gl.h"
#include "glib.h"
#include "primitives.h"
#include "src/core/matrix.h"
#include "src/core/modifier.h"
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
    double err = 0.0025;
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

void vektor_vb_rasterize(VertexBuffer* vb, VektorShapeNodeBuffer* nodebuf,
                         double scale) {
    for (size_t i = 0; i < nodebuf->count; i++) {
        EdgeBuffer edges = {0};

        vektor_shapenode_update(&nodebuf->nodes[i]);
        VektorShape* currentShape = vektor_shapenode_get_evaluated(&nodebuf->nodes[i]);

        VektorPrimitive* p = &currentShape->primitive;
        VektorStyle style = currentShape->style;
        M33 transform = currentShape->transform;

        switch (p->kind) {
        case VEKTOR_POLYLINE:
            vektor_polyline_tessellate(&edges, p->polyline, i, scale);
            vektor_edges_to_triangles(vb, &edges, &transform, style, FALSE);
            break;

        case VEKTOR_POLYGON:
            vektor_polygon_tessellate(&edges, p->polygon, i, scale);
            vektor_edges_to_triangles(vb, &edges, &transform, style, TRUE);
            break;

        case VEKTOR_CIRCLE:
            vektor_circle_tessellate(&edges, &p->circle, i, scale);
            vektor_edges_to_triangles(vb, &edges, &transform, style, TRUE);
            break;

        case VEKTOR_RECTANGLE:
            vektor_rectangle_tessellate(&edges, &p->rectangle, i, scale);
            vektor_edges_to_triangles(vb, &edges, &transform, style, TRUE);
            break;

        default:
            // TODO: fill in all primitives
            break;
        }
    }
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

Edge edge_transform(const Edge* e, const M33* t) {
    Edge out = *e;
    out.p1 = m33_transform(*t, e->p1);
    out.p2 = m33_transform(*t, e->p2);
    return out;
}

V2 line_intersection(V2 p, V2 r, V2 q, V2 s) {
    float t = vec2_cross(vec2_sub(q, p), s) / vec2_cross(r, s);
    return vec2_add(p, vec2_scale(r, t));
}

void vektor_edges_to_triangles(VertexBuffer* vb, EdgeBuffer* edges,
                               M33* transform, VektorStyle style, bool closed) {
    if (!edges || edges->count < 1)
        return;

    float hw = style.stroke_width * 0.5f;

    for (size_t i = 0; i < edges->count; i++) {
        Edge e = edge_transform(&edges->edges[i], transform);

        V2 d = vec2_normalize(vec2_sub(e.p2, e.p1));
        V2 n = vec2_perp(d);
        V2 off = vec2_scale(n, hw);

        V2 v0 = vec2_add(e.p1, off);
        V2 v1 = vec2_sub(e.p1, off);
        V2 v2 = vec2_add(e.p2, off);
        V2 v3 = vec2_sub(e.p2, off);

        vektor_vb_add_triangle(vb, v0, v1, v2, style.stroke_color);
        vektor_vb_add_triangle(vb, v2, v1, v3, style.stroke_color);
    }

    size_t limit = closed ? edges->count : edges->count - 1;

    for (size_t i = 0; i < limit; i++) {
        Edge e1 = edge_transform(&edges->edges[i], transform);
        Edge e2 =
            edge_transform(&edges->edges[(i + 1) % edges->count], transform);

        V2 corner = e1.p2;

        V2 d1 = vec2_normalize(vec2_sub(e1.p2, e1.p1));
        V2 d2 = vec2_normalize(vec2_sub(e2.p2, e2.p1));

        V2 n1 = vec2_perp(d1);
        V2 n2 = vec2_perp(d2);

        V2 off1 = vec2_scale(n1, hw);
        V2 off2 = vec2_scale(n2, hw);

        V2 v10 = vec2_add(e1.p1, off1);
        V2 v11 = vec2_sub(e1.p1, off1);
        V2 v12 = vec2_add(e1.p2, off1);
        V2 v13 = vec2_sub(e1.p2, off1);

        V2 v20 = vec2_add(e2.p1, off2);
        V2 v21 = vec2_sub(e2.p1, off2);
        V2 v22 = vec2_add(e2.p2, off2);
        V2 v23 = vec2_sub(e2.p2, off2);

        V2 outer1 = vec2_add(corner, off1);
        V2 outer2 = vec2_add(corner, off2);

        V2 inner1 = vec2_sub(corner, off1);
        V2 inner2 = vec2_sub(corner, off2);

        float cos_theta = vec2_dot(d1, d2);
        float sin_half = sqrtf((1.0f - cos_theta) * 0.5f);
        float miter_len = hw / sin_half;

        if (miter_len > 4.0 * hw || miter_len < 1.05 * hw) {
            vektor_vb_add_triangle(vb, outer1, corner, outer2,
                                   style.stroke_color);
            vektor_vb_add_triangle(vb, inner1, corner, inner2,
                                   style.stroke_color);
            continue;
        }

        V2 outer_miter = line_intersection(vec2_add(corner, off1), d1,
                                           vec2_add(corner, off2), d2);

        V2 inner_miter = line_intersection(vec2_sub(corner, off1), d1,
                                           vec2_sub(corner, off2), d2);

        V2 mo = vec2_sub(outer_miter, corner);
        V2 mi = vec2_sub(inner_miter, corner);

        V2 vo1 = line_intersection(corner, vec2_normalize(vec2_perp(mo)),
                                   vec2_add(corner, off1), d1);
        V2 vo2 = line_intersection(corner,
                                   vec2_negate(vec2_normalize(vec2_perp(mo))),
                                   vec2_add(corner, off2), d2);

        V2 vi1 = line_intersection(corner, vec2_normalize(vec2_perp(mi)),
                                   vec2_sub(corner, off1), d1);
        V2 vi2 = line_intersection(corner,
                                   vec2_negate(vec2_normalize(vec2_perp(mi))),
                                   vec2_sub(corner, off2), d2);

        vektor_vb_add_triangle(vb, vo1, outer_miter, vo2, style.stroke_color);
        vektor_vb_add_triangle(vb, vi1, inner_miter, vi2, style.stroke_color);
    }
}