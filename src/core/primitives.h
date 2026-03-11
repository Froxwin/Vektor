#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "src/core/matrix.h"
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
    M33 transform;
    VektorBBox bbox;
    VektorPrimitive primitive;

    V2* handles;
    size_t handleCount;
} VektorShape;

VektorPolyline* vektor_polyline_new(void);
void vektor_polyline_add_point(VektorPolyline* pl, V2 point);
void vektor_polyline_free(VektorPolyline* pl);

VektorPolygon* vektor_polygon_new(void);
void vektor_polygon_add_point(VektorPolygon* pl, V2 point);
void vektor_polygon_free(VektorPolygon* pl);

VektorCircle* vektor_circle_new(void);
void vektor_circle_set_center(VektorCircle* circle, V2 point);
void vektor_circle_set_radius(VektorCircle* circle, double radius);
void vektor_circle_free(VektorCircle* circle);

VektorRectangle* vektor_rectangle_new(void);
void vektor_rectangle_set_end(VektorRectangle* rct, V2 point);
void vektor_rectangle_set_start(VektorRectangle* rct, V2 point);
void vektor_rectangle_free(VektorRectangle* rct);

VektorShape vektor_shape_new(VektorPrimitive prim, VektorStyle style,
                             int z_index);

VektorBBox vektor_polyline_get_bbox(VektorPrimitive prim);
VektorBBox vektor_polygon_get_bbox(VektorPrimitive prim);
VektorBBox vektor_circle_get_bbox(VektorPrimitive prim);
VektorBBox vektor_rectangle_get_bbox(VektorPrimitive prim);

VektorBBox vektor_primitive_get_bbox(VektorPrimitive prim);
bool vektor_bbox_isinside(VektorBBox bbox, V2 point);
VektorBBox vektor_bbox_fromcenter(V2 center, float dist);
VektorBBox vektor_bbox_expand(VektorBBox bbox, float val);

// shape handles
void vektor_polyline_create_handles(VektorPolyline* polyline, V2** handleArr,
                                    size_t* count);
void vektor_polygon_create_handles(VektorPolygon* polygon, V2** handleArr,
                                   size_t* count);
void vektor_circle_create_handles(VektorCircle* circle, V2** handleArr,
                                  size_t* count);
void vektor_rectangle_create_handles(VektorRectangle* rectangle, V2** handleArr,
                                     size_t* count);
void vektor_shape_create_handles(VektorShape* shape);
void vektor_shape_add_handle(VektorShape* shape, V2 handle);

/* reconstructs the shape based on handles alone */
void vektor_polyline_handles_updated(VektorPolyline* polyline, V2** handles,
                                     size_t* count);
void vektor_polygon_handles_updated(VektorPolygon* polygon, V2** handles,
                                    size_t* count);
void vektor_circle_handles_updated(VektorCircle* circle, V2** handles,
                                   size_t* count);
void vektor_rectangle_handles_updated(VektorRectangle* rectangle, V2** handles,
                                      size_t* count);
void vektor_shape_handles_updated(VektorShape* shape);

typedef struct {
    VektorShape* shapes;
    size_t count;
    size_t capacity;
} VektorShapeBuffer;

void vektor_shapebuffer_add_shape(VektorShapeBuffer* buffer, VektorShape shape);
void vektor_shapes_update_bbox(VektorShapeBuffer* buffer);
#endif // PRIMITIVES_H_
