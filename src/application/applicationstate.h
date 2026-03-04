#ifndef VKTR_APPSTATE_H
#define VKTR_APPSTATE_H

#include "../core/primitives.h"
#include "../ui/uicontroller.h"
#include "../ui/vektorcanvas.h"
#include "src/core/raster.h"

typedef enum VektorAppTool { VektorLineTool } VektorAppTool;

typedef struct VektorAppState {
    VektorAppTool selectedTool;
    VektorPrimitive *selectedPrimitive;

    // Logic space
    VektorPrimitiveBuffer *primitiveBuffer;
    // Pixel space
    VektorFramebuffer *frameBuffer;
    // View space
    VektorCanvas *canvas;

} VektorAppState;

void vektor_appstate_new(VektorWidgetState *wstate, VektorAppState *stateOut);
void vektor_appstate_canvas_click(VektorAppState *state, double x, double y);

#endif