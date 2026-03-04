#include "./applicationstate.h"
#include "src/core/primitives.h"
#include "src/core/raster.h"
#include "src/ui/vektorcanvas.h"

typedef struct button_tool_set_data {
    VektorAppState* state;
    VektorAppTool tool;
} button_tool_set_data;

static void appstate_set_tool(GtkButton* button, gpointer user_data) {
    button_tool_set_data* data = (button_tool_set_data*)user_data;
    data->state->selectedTool = data->tool;

    // setting tool also resets selected primitive
    data->state->selectedPrimitive = NULL;
}

static void canvas_onclick(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data) {
    vektor_appstate_canvas_click(user_data, x, y);
}

void vektor_appstate_canvas_click(VektorAppState* state, double x, double y) {
    V2 pos = (V2){x,y};

    begin_click_dispatch:
    if(state->selectedTool == VektorLineTool) {
        // create new polyline primitive if none is selected
        if(state->selectedPrimitive == NULL) {
            
            VektorPolyline* line = vektor_polyline_new();
            VektorPrimitive linePrimitive = (VektorPrimitive){
                .kind = VEKTOR_POLYLINE, .polyline = line
            };
            vektor_primitivebuffer_add_primitive(state->primitiveBuffer, linePrimitive);
            
            state->selectedPrimitive = 
                &(state->primitiveBuffer->primitives[state->primitiveBuffer->count - 1]);
            
        } else if (state->selectedPrimitive->kind != VEKTOR_POLYLINE) {
            // selecting a tool resets the selection, so this condition
            // should not happen
            g_warning("Invalid selected primitive; polyline expected");
            state->selectedPrimitive = NULL;
            goto begin_click_dispatch; // retry
        }
        
        
        vektor_polyline_add_point(state->selectedPrimitive->polyline, pos);
    }

    vektor_framebuffer_rasterize(state->frameBuffer, state->primitiveBuffer);
    vektor_canvas_drawfrom(state->frameBuffer, state->canvas);
    vektor_canvas_update(state->canvas);
}

void vektor_appstate_new(VektorWidgetState* wstate, VektorAppState* stateOut) {
    button_tool_set_data* data_linetool = malloc(sizeof(button_tool_set_data));
    data_linetool->state = stateOut;
    data_linetool->tool = VektorLineTool;

    // populate appstate
    stateOut->primitiveBuffer = malloc(sizeof(VektorPrimitiveBuffer));
    *stateOut->primitiveBuffer = (VektorPrimitiveBuffer){0};
    stateOut->frameBuffer = malloc(sizeof(VektorFramebuffer));
    *stateOut->frameBuffer = vektor_framebuffer_new(400, 400);
    stateOut->canvas = malloc(sizeof(VektorCanvas));
    vektor_canvas_init(wstate, stateOut->canvas);

    // link all the buttons
    g_signal_connect(
        G_OBJECT(wstate->workspaceButtonLinetool), 
        "clicked", G_CALLBACK(appstate_set_tool), data_linetool);

    // Add click gesture to canvas
    GtkGesture* canvasClickGesture = gtk_gesture_click_new();
    g_signal_connect(
        G_OBJECT(canvasClickGesture), 
        "pressed", G_CALLBACK(canvas_onclick), stateOut
    );
    gtk_widget_add_controller(
        GTK_WIDGET(wstate->workspaceCanvas), 
        GTK_EVENT_CONTROLLER(canvasClickGesture)
    );
}