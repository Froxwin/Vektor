#ifndef VKTR_CANVAS_H
#define VKTR_CANVAS_H

#include "../core/raster.h"
#include "../util/color.h"
#include "gtk/gtk.h"
#include "src/core/matrix.h"
#include "src/core/modifier.h"
#include "src/core/primitives.h"
#include "uicontroller.h"

typedef struct VektorCanvas {
    GtkGLArea* canvasWidget;

    // texture related stuff
    guchar* canvasPixels;
    GdkTexture* canvasTexture;
    GBytes* canvasPixelBytes;

    int width;
    int height;
} VektorCanvas;

typedef struct VektorCanvasRenderInfo {
    gint64 startupTime;
    VektorShapeNodeBuffer* shapes;

    // a pointer to appstate->selectedShape
    VektorShapeNode** selectedShape;
    float zoom;
    float panX;
    float panY;
    float rotation;

    float dragStartRotation;
    double dragStartAngle;
    double drag_start_x;
    double drag_start_y;
    double mouse_start_x;
    double mouse_start_y;

    M33 canvasMat;
    float canvasTransform[16];
} VektorCanvasRenderInfo;

void vektor_canvas_init(VektorWidgetState* state, VektorCanvas* canvasOut,
                        VektorCanvasRenderInfo* renderInfo);
void vektor_canvas_geometry_changed(VektorCanvasRenderInfo* renderInfo);
// void vektor_canvas_update(VektorCanvas* canvas);
// void vektor_canvas_fill(VektorCanvas* canvas, VektorColor color);
// void vektor_canvas_drawfrom(VektorFramebuffer* fb, VektorCanvas* canvas);

#endif