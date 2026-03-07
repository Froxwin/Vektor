#ifndef VKTR_APPSTATE_H
#define VKTR_APPSTATE_H

#include "../core/primitives.h"
#include "../ui/uicontroller.h"
#include "../ui/vektorcanvas.h"
#include "src/core/raster.h"

typedef enum VektorAppTool { VektorLineTool } VektorAppTool;

typedef struct VektorAppState {
    VektorWidgetState* widgetState;

    VektorAppTool selectedTool;
    VektorShape* selectedShape;

    VektorColor currentColor;

    // Logic space
    VektorShapeBuffer* shapeBuffer;
    // View space
    VektorCanvas* canvas;

} VektorAppState;

void vektor_appstate_new(VektorWidgetState* wstate, VektorAppState* stateOut);
void vektor_appstate_canvas_click(VektorAppState* state, double x, double y);

#endif