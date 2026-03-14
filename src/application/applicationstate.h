#ifndef VKTR_APPSTATE_H
#define VKTR_APPSTATE_H

#include "../core/primitives.h"
#include "../ui/uicontroller.h"
#include "../ui/vektorcanvas.h"
#include "src/core/modifier.h"

typedef enum VektorAppTool {
    VektorSelectionTool,
    VektorLineTool,
    VektorPolygonTool,
    VektorRectangleTool,
    VektorCircleTool
} VektorAppTool;

typedef struct VektorAppState {
    gint64 startupTime;

    VektorWidgetState* widgetState;

    VektorAppTool selectedTool;
    VektorShapeNode* selectedShape;
    int heldHandleIndex;

    VektorColor currentColor;

    // Logic space
    VektorShapeNodeBuffer* shapeBuffer;
    // View space
    VektorCanvas* canvas;
    VektorCanvasRenderInfo* renderInfo;
} VektorAppState;

void vektor_appstate_new(VektorWidgetState* wstate, VektorAppState* stateOut);
void vektor_appstate_canvas_click(VektorAppState* state, double x, double y);
void vektor_appstate_deselect_shape(VektorAppState* state);

#endif