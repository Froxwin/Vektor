#ifndef VKTR_CANVAS_H
#define VKTR_CANVAS_H

#include "uicontroller.h"

typedef struct VektorCanvas {
    GtkPicture* canvasWidget;

    // texture related stuff
    guchar* canvasPixels;
    GdkTexture* canvasTexture;
    GBytes* canvasPixelBytes;

    int width;
    int height;
} VektorCanvas;

typedef struct VektorCanvasColor {
    guchar r;
    guchar g;
    guchar b;
    guchar a;
} VektorCanvasColor;

void vektor_canvas_init(VektorWidgetState* state, VektorCanvas* canvasOut);
void vektor_canvas_update(VektorCanvas* canvas);
void vektor_canvas_fill(VektorCanvas* canvas, VektorCanvasColor* color);

VektorCanvasColor vektor_color_new(guchar r, guchar g, guchar b, guchar a);

#endif