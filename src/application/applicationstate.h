#ifndef VKTR_APPSTATE_H
#define VKTR_APPSTATE_H

#include "../ui/uicontroller.h"
#include "../core/primitives.h"
#include "src/core/raster.h"
#include "../ui/vektorcanvas.h"

typedef enum VektorAppTool {
    VektorLineTool
} VektorAppTool;

typedef struct VektorAppState {
    VektorAppTool selectedTool;
    VektorPrimitive* selectedPrimitive;

    // Logic space
    VektorPrimitiveBuffer* primitiveBuffer;
    // Pixel space
    VektorFramebuffer* frameBuffer;
    // View space
    VektorCanvas* canvas;

} VektorAppState;

void vektor_appstate_new(VektorWidgetState* wstate, VektorAppState* stateOut);
void vektor_appstate_canvas_click(VektorAppState* state, double x, double y);

#endif