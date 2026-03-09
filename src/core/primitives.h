#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "src/util/color.h"
#include "stddef.h"
#include "stdlib.h"
#include "vector.h"

typedef struct {
    V2* points;
    size_t count;
    size_t capacity;
} VektorPolyline;

typedef struct {
    V2* points;
    size_t count;
    size_t capacity;
} VektorPolygon;

typedef struct {
    V2 center;
    double radius;
} VektorCircle;

typedef struct {
    V2 start;
    V2 end;
} VektorRectangle;

typedef enum {
    VEKTOR_POLYLINE,
    VEKTOR_POLYGON,
    VEKTOR_CIRCLE,
    VEKTOR_RECTANGLE
} VektorPrimitiveKind;

typedef struct {
    VektorPrimitiveKind kind;
    union {
        VektorPolyline* polyline;
        VektorPolygon* polygon;
        VektorCircle circle;
        VektorRectangle rectangle;
    };
} VektorPrimitive;

VektorPolyline* vektor_polyline_new(void);
void vektor_polyline_add_point(VektorPolyline* pl, V2 point);
void vektor_polyline_free(VektorPolyline* pl);

VektorPolygon* vektor_polygon_new(void);
void vektor_polygon_add_point(VektorPolygon* pl, V2 point);
void vektor_polygon_free(VektorPolygon* pl);

VektorRectangle* vektor_rectangle_new(void);
void vektor_rectangle_set_end(VektorRectangle* rct, V2 point);
void vektor_rectangle_set_start(VektorRectangle* rct, V2 point);
void vektor_rectangle_free(VektorRectangle* rct);

typedef struct {
    VektorColor stroke_color;
    float stroke_width;
} VektorStyle;

typedef struct {
    V2 min;
    V2 max;
} VektorBBox;

typedef struct {
    VektorStyle style;
    int z_index;
    VektorBBox bbox;
    VektorPrimitive primitive;
} VektorShape;

VektorBBox vektor_polyline_get_bbox(VektorPrimitive prim);
VektorBBox vektor_polygon_get_bbox(VektorPrimitive prim);
VektorBBox vektor_rectangle_get_bbox(VektorPrimitive prim);

VektorBBox vektor_primitive_get_bbox(VektorPrimitive prim);

VektorShape vektor_shape_new(VektorPrimitive prim, VektorStyle style,
                             int z_index);

typedef struct {
    VektorShape* shapes;
    size_t count;
    size_t capacity;
} VektorShapeBuffer;

void vektor_shapebuffer_add_shape(VektorShapeBuffer* buffer, VektorShape shape);
void vektor_shapes_update_bbox(VektorShapeBuffer* buffer);
#endif // PRIMITIVES_H_
