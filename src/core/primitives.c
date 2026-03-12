#include "primitives.h"
#include "glib.h"
#include "src/core/matrix.h"
#include "src/core/vector.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>

// ------ PER-PRIMITIVE METHODS ------

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

    if (pl->count <= pl->capacity / 4) {
        pl->capacity /= 2;
        pl->points = realloc(pl->points, sizeof(V2) * pl->capacity);
    }
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

    if (pg->count <= pg->capacity / 4) {
        pg->capacity /= 2;
        pg->points = realloc(pg->points, sizeof(V2) * pg->capacity);
    }
}

void vektor_polygon_free(VektorPolygon* pg) {
    if (!pg)
        return;
    free(pg->points);
    free(pg);
}

VektorCircle* vektor_circle_new(void) {
    VektorCircle* circ = malloc(sizeof(VektorCircle));
    circ->center = (V2){0, 0};
    circ->radius = 0;
    return circ;
}

void vektor_circle_set_center(VektorCircle* circle, V2 point) {
    circle->center = point;
}

void vektor_circle_set_radius(VektorCircle* circle, double radius) {
    circle->radius = radius;
}

void vektor_circle_free(VektorCircle* circle) { free(circle); }

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

void vektor_rectangle_free(VektorRectangle* rct) { free(rct); }

VektorBBox vektor_polyline_get_bbox(VektorPrimitive prim) {
    V2 first = prim.polyline->points[0];

    float min_x = first.x;
    float max_x = first.x;
    float min_y = first.y;
    float max_y = first.y;

    for (size_t i = 1; i < prim.polygon->count; i++) {
        V2 p = prim.polygon->points[i];

        min_x = fminf(min_x, p.x);
        min_y = fminf(min_y, p.y);

        max_x = fmaxf(max_x, p.x);
        max_y = fmaxf(max_y, p.y);
    }

    return (VektorBBox){(V2){min_x, min_y}, (V2){max_x, max_y}};
}

VektorBBox vektor_polygon_get_bbox(VektorPrimitive prim) {
    V2 first = prim.polygon->points[0];

    float min_x = first.x;
    float max_x = first.x;
    float min_y = first.y;
    float max_y = first.y;

    for (size_t i = 1; i < prim.polygon->count; i++) {
        V2 p = prim.polygon->points[i];

        min_x = fminf(min_x, p.x);
        min_y = fminf(min_y, p.y);

        max_x = fmaxf(max_x, p.x);
        max_y = fmaxf(max_y, p.y);
    }

    return (VektorBBox){(V2){min_x, min_y}, (V2){max_x, max_y}};
}

VektorBBox vektor_rectangle_get_bbox(VektorPrimitive prim) {
    return (VektorBBox){prim.rectangle.start, prim.rectangle.end};
}

VektorBBox vektor_circle_get_bbox(VektorPrimitive prim) {
    return (VektorBBox){
        vec2_sub(prim.circle.center, vec2_fromfloat(prim.circle.radius)),
        vec2_add(prim.circle.center, vec2_fromfloat(prim.circle.radius))};
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

    case VEKTOR_CIRCLE:
        return vektor_circle_get_bbox(prim);
        break;

    default:
        // TODO: fill in all primitives
        break;
    }
}

// ------ PRIMITIVE HANDLES GENERATION ------

/* [n]: polyline vertices */
void vektor_polyline_create_handles(VektorPolyline* polyline, V2** handleArr,
                                    size_t* count) {
    *count = 0;
    *handleArr = NULL;
}

/* [n]: polygon vertices */
void vektor_polygon_create_handles(VektorPolygon* polygon, V2** handleArr,
                                   size_t* count) {
    *count = 0;
    *handleArr = NULL;
}

/* [0]: center; [1]: radius */
void vektor_circle_create_handles(VektorCircle* circle, V2** handleArr,
                                  size_t* count) {
    *count = 2;
    *handleArr = (V2*)malloc(sizeof(V2) * (*count));
    (*handleArr)[0] = circle->center;
    (*handleArr)[1] = (V2){circle->radius + circle->center.x, circle->center.y};
}

/* [0]: center; [1-4]: corners (l2r, t2b); */
void vektor_rectangle_create_handles(VektorRectangle* rectangle, V2** handleArr,
                                     size_t* count) {
    *count = 5;
    free(*handleArr);
    *handleArr = (V2*)malloc(sizeof(V2) * (*count));

    V2 halfdist = vec2_scale(vec2_sub(rectangle->end, rectangle->start), 0.5f);
    V2 center = vec2_add(rectangle->start, halfdist);

    (*handleArr)[0] = center;
    (*handleArr)[1] = vec2_add(center, vec2_mul(halfdist, (V2){-1.0f, 1.0f}));
    (*handleArr)[2] = vec2_add(center, halfdist);
    (*handleArr)[3] = vec2_add(center, vec2_mul(halfdist, (V2){-1.0f, -1.0f}));
    (*handleArr)[4] = vec2_add(center, vec2_mul(halfdist, (V2){1.0f, -1.0f}));
}

void vektor_shape_create_handles(VektorShape* shape) {
    switch (shape->primitive.kind) {
    case VEKTOR_POLYLINE:
        vektor_polyline_create_handles(shape->primitive.polyline,
                                       &shape->handles, &shape->handleCount);
        break;
    case VEKTOR_POLYGON:
        vektor_polygon_create_handles(shape->primitive.polygon, &shape->handles,
                                      &shape->handleCount);
        break;
    case VEKTOR_CIRCLE:
        vektor_circle_create_handles(&shape->primitive.circle, &shape->handles,
                                     &shape->handleCount);
        break;
    case VEKTOR_RECTANGLE:
        vektor_rectangle_create_handles(&shape->primitive.rectangle,
                                        &shape->handles, &shape->handleCount);
        break;
    }
}

// ------ AUXILIARY HANDLE METHODS ------


void vektor_shape_add_handle(VektorShape* shape, V2 handle) {
    // could be optimised with capacity property
    // but this function is only called when adding new
    // points to polyline and polygon, so it should
    // not be that much of an overhead
    shape->handles =
        realloc(shape->handles, sizeof(V2) * shape->handleCount + 1);
    shape->handles[shape->handleCount++] = handle;
}

VektorBBox vektor_shape_get_handle_bbox(V2 handle) {
    return vektor_bbox_fromcenter(handle, 0.02);
}

// ------ PRIMITIVE HANDLES UPDATING ------

void vektor_polyline_handles_updated(VektorPolyline* polyline, V2** handles,
                                     size_t* count, int* heldHandleIndex) {
    if (*count != polyline->count) {
        g_warning("handle count & point count mismatch in polyline");
        return;
    }
    for (size_t i = 0; i < *count; i++) {
        polyline->points[i] = (*handles)[i];
    }
}

void vektor_polygon_handles_updated(VektorPolygon* polygon, V2** handles,
                                    size_t* count, int* heldHandleIndex) {
    if (*count != polygon->count) {
        g_warning("handle count & point count mismatch in polygon");
        return;
    }
    for (size_t i = 0; i < *count; i++) {
        polygon->points[i] = (*handles)[i];
    }
}

void vektor_circle_handles_updated(VektorCircle* circle, V2** handles,
                                   size_t* count, int* heldHandleIndex) {
    if (*count != 2) {
        g_warning("unexpected circle handle count (%zu)", *count);
        return;
    }

    if(*heldHandleIndex == 0) { // dragging center
        V2 translation = vec2_sub((*handles)[0], circle->center);
        circle->center = (*handles)[0];
        (*handles)[1] = vec2_add(translation, (*handles)[1]);
    }

    circle->center = (*handles)[0];
    circle->radius = vec2_length(vec2_sub((*handles)[0], (*handles)[1]));
}

// this shi is big because it dynamically handles handle remapping when
// rectangle enters an invalid state (end < start)
// creating the illusion of an invertable rect, while also keeping it
// valid at all times
void vektor_rectangle_handles_updated(VektorRectangle* rectangle, V2** handles, size_t* count, int* heldHandleIndex) {
    if (*count != 5) {
        g_warning("unexpected rectangle handle count (%zu)", *count);
        return;
    }

    V2 start = rectangle->start;
    V2 end   = rectangle->end;

    switch (*heldHandleIndex)
    {
        case 0: // center drag
        {
            V2 oldCenter = vec2_scale(vec2_add(start, end), 0.5f);
            V2 newCenter = (*handles)[0];

            V2 translation = vec2_sub(newCenter, oldCenter);

            start = vec2_add(start, translation);
            end   = vec2_add(end, translation);
            break;
        }

        case 1: // top-left
            start.x = (*handles)[1].x;
            end.y   = (*handles)[1].y;
            break;

        case 2: // top-right
            end.x = (*handles)[2].x;
            end.y = (*handles)[2].y;
            break;

        case 3: // bottom-left
            start.x = (*handles)[3].x;
            start.y = (*handles)[3].y;
            break;

        case 4: // bottom-right
            end.x   = (*handles)[4].x;
            start.y = (*handles)[4].y;
            break;

        default:
            return;
    }

    // Store raw values before normalization
    float raw_min_x = start.x;
    float raw_max_x = end.x;
    float raw_min_y = start.y;
    float raw_max_y = end.y;

    // Normalize rectangle
    float min_x = fminf(start.x, end.x);
    float max_x = fmaxf(start.x, end.x);
    float min_y = fminf(start.y, end.y);
    float max_y = fmaxf(start.y, end.y);

    bool flipX = raw_min_x > raw_max_x;
    bool flipY = raw_min_y > raw_max_y;

    // Remap handle if we crossed axes
    if (*heldHandleIndex != 0)
    {
        if (flipX) {
            switch (*heldHandleIndex) {
                case 1: *heldHandleIndex = 2; break;
                case 2: *heldHandleIndex = 1; break;
                case 3: *heldHandleIndex = 4; break;
                case 4: *heldHandleIndex = 3; break;
            }
        }

        if (flipY) {
            switch (*heldHandleIndex) {
                case 1: *heldHandleIndex = 3; break;
                case 3: *heldHandleIndex = 1; break;
                case 2: *heldHandleIndex = 4; break;
                case 4: *heldHandleIndex = 2; break;
            }
        }
    }

    VektorRectangle properRect = {
        .start = {min_x, min_y},
        .end   = {max_x, max_y}
    };

    vektor_rectangle_set_start(rectangle, properRect.start);
    vektor_rectangle_set_end(rectangle, properRect.end);

    // regenerate handle positions
    vektor_rectangle_create_handles(&properRect, handles, count);
}

void vektor_shape_handles_updated(VektorShape* shape, int* heldHandleIndex) {
    switch (shape->primitive.kind) {
    case VEKTOR_POLYLINE:
        vektor_polyline_handles_updated(shape->primitive.polyline,
                                        &shape->handles, &shape->handleCount, heldHandleIndex);
        break;
    case VEKTOR_POLYGON:
        vektor_polygon_handles_updated(shape->primitive.polygon,
                                       &shape->handles, &shape->handleCount, heldHandleIndex);
        break;
    case VEKTOR_CIRCLE:
        vektor_circle_handles_updated(&shape->primitive.circle, &shape->handles,
                                      &shape->handleCount, heldHandleIndex);
        break;
    case VEKTOR_RECTANGLE:
        vektor_rectangle_handles_updated(&shape->primitive.rectangle,
                                         &shape->handles, &shape->handleCount, heldHandleIndex);
        break;
    }
}

// ------ BBOX METHODS ------

bool vektor_bbox_isinside(VektorBBox bbox, V2 point) {
    return point.x >= bbox.min.x && point.y >= bbox.min.y &&
           point.x <= bbox.max.x && point.y <= bbox.max.y;
}

VektorBBox vektor_bbox_fromcenter(V2 center, float dist) {
    V2 v2dist = vec2_fromfloat(dist);
    V2 min = vec2_sub(center, v2dist);
    V2 max = vec2_add(center, v2dist);
    return (VektorBBox){min, max};
}

VektorBBox vektor_bbox_expand(VektorBBox bbox, float val) {
    return (VektorBBox){vec2_sub(bbox.min, vec2_fromfloat(val)),
                        vec2_add(bbox.max, vec2_fromfloat(val))};
}

// ------ SHAPE METHODS ------

VektorShape vektor_shape_new(VektorPrimitive prim, VektorStyle style,
                             int z_index) {
    VektorShape shape = (VektorShape){.primitive = prim,
                                      .style = style,
                                      .transform = m33_identity(),
                                      .z_index = z_index,
                                      .bbox = vektor_primitive_get_bbox(prim)};

    /*
    create_handles() allocates new buffer for handles,
    and even if the local shape variable goes out of scope and deallocates,
    the passed value's pointer to an array of handles remains valid in the
    passed copy.
    */
    vektor_shape_create_handles(&shape);
    return shape;
}

void vektor_shapes_update_bbox(VektorShapeBuffer* buffer) {
    for (size_t i = 0; i < buffer->count; i++) {
        buffer->shapes[i].bbox =
            vektor_primitive_get_bbox(buffer->shapes[i].primitive);
    }
}

void vektor_shapebuffer_add_shape(VektorShapeBuffer* buffer,
                                  VektorShape shape) {
    if (buffer->count >= buffer->capacity) {
        buffer->capacity = buffer->capacity ? buffer->capacity * 2 : 4;
        buffer->shapes =
            realloc(buffer->shapes, sizeof(VektorShape) * buffer->capacity);
    }
    buffer->shapes[buffer->count++] = shape;

    if (buffer->count <= buffer->capacity / 4) {
        buffer->capacity /= 2;
        buffer->shapes =
            realloc(buffer->shapes, sizeof(VektorShape) * buffer->capacity);
    }
}