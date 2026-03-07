#ifndef VKTR_CANVAS_H
#define VKTR_CANVAS_H

#include "../core/raster.h"
#include "../util/color.h"
#include "gtk/gtk.h"
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

void vektor_canvas_init(VektorWidgetState* state, VektorCanvas* canvasOut,
                        VektorShapeBuffer* shapes);
// void vektor_canvas_update(VektorCanvas* canvas);
// void vektor_canvas_fill(VektorCanvas* canvas, VektorColor color);
// void vektor_canvas_drawfrom(VektorFramebuffer* fb, VektorCanvas* canvas);

#endif