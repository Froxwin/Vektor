#include "./applicationstate.h"
#include "glib.h"
#include "gtk/gtk.h"
#include "gtk/gtkrevealer.h"
#include "src/core/primitives.h"
#include "src/core/raster.h"
#include "src/ui/vektorcanvas.h"
#include "src/ui/widgets/colorwheel.h"
#include "src/util/color.h"

typedef struct button_tool_set_data {
    GtkRevealer* revealer;
    VektorAppState* state;
    VektorAppTool tool;
} button_tool_set_data;

static void appstate_set_tool(GtkButton* button, gpointer user_data) {
    button_tool_set_data* data = (button_tool_set_data*)user_data;
    data->state->selectedTool = data->tool;

    // setting tool makes the sub-tools menu to close
    gtk_revealer_set_reveal_child(data->revealer, FALSE);

    // setting tool also resets selected shape
    data->state->selectedShape = NULL;
}

static void appstate_reveal_subtools(GtkButton* button, gpointer user_data) {
    GtkRevealer* revealer = (GtkRevealer*)user_data;
    gboolean visible = gtk_revealer_get_reveal_child(revealer);
    gtk_revealer_set_reveal_child(revealer, !visible);
}

static void appstate_on_color_change(VektorColorWheel* wheel, gpointer user_data) {
    VektorColor c = vektor_color_wheel_get_color(wheel);
    VektorAppState* appstate = (VektorAppState*)user_data;
    appstate->currentColor = c;

    if(appstate->selectedShape != NULL) {
        appstate->selectedShape->style.stroke_color = c;
    }
    
    gtk_gl_area_queue_render(GTK_GL_AREA(appstate->widgetState->workspaceCanvas));
}

static void canvas_onclick(GtkGestureClick* gesture, int n_press, double x,
                           double y, gpointer user_data) {

    VektorAppState* state = user_data;

    GtkWidget* widget =
        gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

    int widget_w = gtk_widget_get_width(widget);
    int widget_h = gtk_widget_get_height(widget);

    int canvas_w = state->canvas->width;
    int canvas_h = state->canvas->height;

    V2 normalized_coords =
        (V2){(2 * (x / widget_w)) - 1, 1 - (2 * (y / widget_h))};

    vektor_appstate_canvas_click(state, normalized_coords.x,
                                 normalized_coords.y);
    gtk_gl_area_queue_render(GTK_GL_AREA(widget));
}

void vektor_appstate_canvas_click(VektorAppState* state, double x, double y) {
    V2 pos = (V2){x, y};

begin_click_dispatch:
    if (state->selectedTool == VektorLineTool) {
        // create new polyline shape if none is selected
        if (state->selectedShape == NULL) {

            VektorPolyline* line = vektor_polyline_new();
            VektorPrimitive linePrimitive =
                (VektorPrimitive){.kind = VEKTOR_POLYLINE, .polyline = line};
            VektorStyle style =
                (VektorStyle){.stroke_color = state->currentColor,
                              .stroke_width = 0.01};
            VektorShape shape = (VektorShape){
                .primitive = linePrimitive, .z_index = 0, .style = style};
            vektor_shapebuffer_add_shape(state->shapeBuffer, shape);

            state->selectedShape =
                &(state->shapeBuffer->shapes[state->shapeBuffer->count - 1]);

        } else if (state->selectedShape->primitive.kind != VEKTOR_POLYLINE) {
            // selecting a tool resets the selection, so this condition
            // should not happen
            g_warning("Invalid selected primitive; polyline expected");
            state->selectedShape = NULL;
            goto begin_click_dispatch; // retry
        }

        vektor_polyline_add_point(state->selectedShape->primitive.polyline,
                                  pos);
    }
}

void vektor_appstate_new(VektorWidgetState* wstate, VektorAppState* stateOut) {
    button_tool_set_data* data_linetool = malloc(sizeof(button_tool_set_data));
    data_linetool->state = stateOut;
    data_linetool->tool = VektorLineTool;
    data_linetool->revealer = wstate->workspaceRevealerShapes;

    // populate appstate
    stateOut->shapeBuffer = malloc(sizeof(VektorShapeBuffer));
    *stateOut->shapeBuffer = (VektorShapeBuffer){0};
    stateOut->canvas = malloc(sizeof(VektorCanvas));
    stateOut->widgetState = wstate;
    stateOut->currentColor = vektor_color_blank;
    vektor_canvas_init(wstate, stateOut->canvas, stateOut->shapeBuffer);

    // link all the buttons
    g_signal_connect(G_OBJECT(wstate->workspaceButtonLinetool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_linetool);
    g_signal_connect(G_OBJECT(wstate->workspaceButtonRecttool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_linetool);
    g_signal_connect(G_OBJECT(wstate->workspaceButtonCircletool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_linetool);

    // hook subtool revealers to their master buttons
    g_signal_connect(G_OBJECT(wstate->workspaceButtonMasterShapes), "clicked",
                     G_CALLBACK(appstate_reveal_subtools),
                     wstate->workspaceRevealerShapes);

    // hook relevant stuff to master color picker
    g_signal_connect(G_OBJECT(wstate->workspaceColorPicker), "color-changed",
                    G_CALLBACK(appstate_on_color_change), stateOut);

    // Add click gesture to canvas
    GtkGesture* canvasClickGesture = gtk_gesture_click_new();
    g_signal_connect(G_OBJECT(canvasClickGesture), "pressed",
                     G_CALLBACK(canvas_onclick), stateOut);
    gtk_widget_add_controller(GTK_WIDGET(wstate->workspaceCanvas),
                              GTK_EVENT_CONTROLLER(canvasClickGesture));
}