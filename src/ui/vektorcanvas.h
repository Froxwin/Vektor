#ifndef VKTR_CANVAS_H
#define VKTR_CANVAS_H

#include "../core/raster.h"
#include "../util/color.h"
#include "gtk/gtk.h"
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
    VektorShapeBuffer* shapes;

    // a pointer to appstate->selectedShape
    VektorShape** selectedShape;
} VektorCanvasRenderInfo;

void vektor_canvas_init(VektorWidgetState* state, VektorCanvas* canvasOut,
                        VektorCanvasRenderInfo* renderInfo);
// void vektor_canvas_update(VektorCanvas* canvas);
// void vektor_canvas_fill(VektorCanvas* canvas, VektorColor color);
// void vektor_canvas_drawfrom(VektorFramebuffer* fb, VektorCanvas* canvas);

#endif