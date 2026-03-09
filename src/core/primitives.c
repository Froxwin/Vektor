#include "primitives.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>

VektorPolyline* vektor_polyline_new(void) {
    VektorPolyline* pl = malloc(sizeof(VektorPolyline));
    pl->count = 0;
    pl->capacity = 4;
    pl->points = malloc(sizeof(V2) * pl->capacity);
    return pl;
}

void vektor_polyline_add_point(VektorPolyline* pl, V2 point) {
    if (pl->count >= pl->capacity) {
        pl->capacity *= 2;
        pl->points = realloc(pl->points, sizeof(V2) * pl->capacity);
    }
    pl->points[pl->count++] = point;
}

void vektor_polyline_free(VektorPolyline* pl) {
    if (!pl)
        return;
    free(pl->points);
    free(pl);
}

VektorPolygon* vektor_polygon_new(void) {
    VektorPolygon* pg = malloc(sizeof(VektorPolygon));
    pg->count = 0;
    pg->capacity = 4;
    pg->points = malloc(sizeof(V2) * pg->capacity);
    return pg;
}

void vektor_polygon_add_point(VektorPolygon* pg, V2 point) {
    if (pg->count >= pg->capacity) {
        pg->capacity *= 2;
        pg->points = realloc(pg->points, sizeof(V2) * pg->capacity);
    }
    pg->points[pg->count++] = point;
}

void vektor_polygon_free(VektorPolygon* pg) {
    if (!pg)
        return;
    free(pg->points);
    free(pg);
}

VektorRectangle* vektor_rectangle_new(void) {
    VektorRectangle* rct = malloc(sizeof(VektorRectangle));
    rct->start = (V2){.x = 0, .y = 0};
    rct->end = (V2){.x = 0, .y = 0};
    return rct;
}

void vektor_rectangle_set_end(VektorRectangle* rct, V2 point) {
    rct->end = point;
}
void vektor_rectangle_set_start(VektorRectangle* rct, V2 point) {
    rct->start = point;
}

void vektor_rectangle_free(VektorRectangle* rct) {
    free(rct);
}

void vektor_shapebuffer_add_shape(VektorShapeBuffer* buffer,
                                  VektorShape shape) {
    if (buffer->count >= buffer->capacity) {
        buffer->capacity = buffer->capacity ? buffer->capacity * 2 : 4;
        buffer->shapes =
            realloc(buffer->shapes, sizeof(VektorShape) * buffer->capacity);
    }
    buffer->shapes[buffer->count++] = shape;
}

VektorBBox vektor_polyline_get_bbox(VektorPrimitive prim) {
    float min_x, max_x, min_y, max_y;
    for (size_t i = 0; i < prim.polyline->count; i++) {
        V2 p = prim.polyline->points[i];
        min_x = fminf(min_x, p.x);
        min_y = fminf(min_y, p.y);

        max_x = fminf(max_x, p.x);
        max_y = fminf(max_y, p.y);
    }
    return (VektorBBox){(V2){min_x, min_y}, (V2){max_x, max_y}};
}

VektorBBox vektor_polygon_get_bbox(VektorPrimitive prim) {
    float min_x, max_x, min_y, max_y;
    for (size_t i = 0; i < prim.polygon->count; i++) {
        V2 p = prim.polygon->points[i];
        min_x = fminf(min_x, p.x);
        min_y = fminf(min_y, p.y);

        max_x = fminf(max_x, p.x);
        max_y = fminf(max_y, p.y);
    }
    return (VektorBBox){(V2){min_x, min_y}, (V2){max_x, max_y}};
}

VektorBBox vektor_rectangle_get_bbox(VektorPrimitive prim) {
    return *(VektorBBox*)&prim.rectangle;
}

VektorBBox vektor_primitive_get_bbox(VektorPrimitive prim) {
    switch (prim.kind) {
    case VEKTOR_POLYLINE:
        return vektor_polyline_get_bbox(prim);
        break;

    case VEKTOR_POLYGON:
        return vektor_polygon_get_bbox(prim);
        break;

    case VEKTOR_RECTANGLE:
        return vektor_rectangle_get_bbox(prim);
        break;

    default:
        // TODO: fill in all primitives
        break;
    }
}

VektorShape vektor_shape_new(VektorPrimitive prim, VektorStyle style,
                             int z_index) {
    return (VektorShape){.primitive = prim, .style = style, .z_index = z_index, .bbox=vektor_primitive_get_bbox(prim)};
}

void vektor_shapes_update_bbox(VektorShapeBuffer* buffer) {
    for (size_t i = 0; i < buffer->count; i++) {
        buffer->shapes[i].bbox = vektor_primitive_get_bbox(buffer->shapes[i].primitive);
    }
}