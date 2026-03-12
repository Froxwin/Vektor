#include "src/core/matrix.h"
#include "src/ui/uicontroller.h"
#include "stdlib.h"

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
    // (ADD NEW REVEALERS HERE)
    gtk_revealer_set_reveal_child(
        data->state->widgetState->workspaceRevealerShapes, FALSE);

    // setting tool also resets selected shape
    // NOTE: isn't needed anymore, as you would
    // want to be able to select & edit existing shapes
    // data->state->selectedShape = NULL;
}

static void appstate_reveal_subtools(GtkButton* button, gpointer user_data) {
    GtkRevealer* revealer = (GtkRevealer*)user_data;
    gboolean visible = gtk_revealer_get_reveal_child(revealer);
    gtk_revealer_set_reveal_child(revealer, !visible);
}

static void appstate_on_color_change(VektorColorWheel* wheel,
                                     gpointer user_data) {
    VektorColor c = vektor_color_wheel_get_color(wheel);
    VektorAppState* appstate = (VektorAppState*)user_data;
    appstate->currentColor = c;

    if (appstate->selectedShape != NULL) {
        appstate->selectedShape->style.stroke_color = c;
    }

    // set entry fields under the color selector
    char *str_r, *str_g, *str_b;
    str_r = g_strdup_printf("%d", c.r);
    str_g = g_strdup_printf("%d", c.g);
    str_b = g_strdup_printf("%d", c.b);
    gtk_editable_set_text(GTK_EDITABLE(appstate->widgetState->sidepanelEntryR),
                          str_r);
    gtk_editable_set_text(GTK_EDITABLE(appstate->widgetState->sidepanelEntryG),
                          str_g);
    gtk_editable_set_text(GTK_EDITABLE(appstate->widgetState->sidepanelEntryB),
                          str_b);

    /*gtk_gl_area_queue_render(
        GTK_GL_AREA(appstate->widgetState->workspaceCanvas));*/
}

static void appstate_on_entry_update(GtkEntry* entry, gpointer user_data) {
    VektorWidgetState* widgetState = (VektorWidgetState*)user_data;
    unsigned char r = (unsigned char)atoi(
        gtk_editable_get_text(GTK_EDITABLE(widgetState->sidepanelEntryR)));
    unsigned char g = (unsigned char)atoi(
        gtk_editable_get_text(GTK_EDITABLE(widgetState->sidepanelEntryG)));
    unsigned char b = (unsigned char)atoi(
        gtk_editable_get_text(GTK_EDITABLE(widgetState->sidepanelEntryB)));

    vektor_color_wheel_set_color(
        VEKTOR_COLOR_WHEEL(widgetState->workspaceColorPicker),
        (VektorColor){.r = r, .g = g, .b = b});
}

static void canvas_onclick(GtkGestureClick* gesture, int n_press, double x,
                           double y, gpointer user_data) {

    VektorAppState* state = user_data;

    GtkWidget* widget =
        gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

    int widget_w = gtk_widget_get_width(widget);
    int widget_h = gtk_widget_get_height(widget);

    V2 normalized_coords =
        (V2){(2 * (x / widget_w)) - 1, 1 - (2 * (y / widget_h))};

    vektor_appstate_canvas_click(state, normalized_coords.x,
                                 normalized_coords.y);

    // gtk_gl_area_queue_render(GTK_GL_AREA(widget));
}

void vektor_appstate_canvas_click(VektorAppState* state, double x, double y) {
    V2 pos =
        m33_transform(m33_inverse(state->renderInfo->canvasMat), (V2){x, y});

begin_click_dispatch:
    if (state->selectedTool == VektorLineTool) {
        // create new polyline shape if none is selected
        if (state->selectedShape == NULL) {

            VektorPolyline* line = vektor_polyline_new();
            VektorPrimitive linePrimitive =
                (VektorPrimitive){.kind = VEKTOR_POLYLINE, .polyline = line};
            VektorStyle style = (VektorStyle){
                .stroke_color = state->currentColor, .stroke_width = 0.01};

            vektor_shapebuffer_add_shape(
                state->shapeBuffer, vektor_shape_new(linePrimitive, style, 0));

            state->selectedShape =
                &(state->shapeBuffer->shapes[state->shapeBuffer->count - 1]);

        } else if (state->selectedShape->primitive.kind != VEKTOR_POLYLINE) {
            // selecting a tool resets the selection, so this condition
            // should not happen
            g_warning("Invalid selected primitive; polyline expected");
            vektor_appstate_deselect_shape(state);
            goto begin_click_dispatch; // retry
        }

        vektor_polyline_add_point(state->selectedShape->primitive.polyline,
                                  pos);
        vektor_shapes_update_bbox(state->shapeBuffer);

        // polyline's handle count is not fixed, so we have to add them manually
        vektor_shape_add_handle(state->selectedShape, pos);

    } else if (state->selectedTool == VektorPolygonTool) {
        // create new polygon shape if none is selected
        if (state->selectedShape == NULL) {

            VektorPolygon* polygon = vektor_polygon_new();
            VektorPrimitive polygonPrimitive =
                (VektorPrimitive){.kind = VEKTOR_POLYGON, .polygon = polygon};
            VektorStyle style = (VektorStyle){
                .stroke_color = state->currentColor, .stroke_width = 0.01};
            vektor_shapebuffer_add_shape(
                state->shapeBuffer,
                vektor_shape_new(polygonPrimitive, style, 0));

            state->selectedShape =
                &(state->shapeBuffer->shapes[state->shapeBuffer->count - 1]);

        } else if (state->selectedShape->primitive.kind != VEKTOR_POLYGON) {
            g_warning("Invalid selected primitive; polygon expected");
            vektor_appstate_deselect_shape(state);
            goto begin_click_dispatch; // retry
        }

        vektor_polygon_add_point(state->selectedShape->primitive.polygon, pos);
        vektor_shapes_update_bbox(state->shapeBuffer);

        // polygon's handle count is not fixed, so we have to add them manually
        vektor_shape_add_handle(state->selectedShape, pos);

    } else if (state->selectedTool == VektorCircleTool) {

        VektorCircle* circle = vektor_circle_new();
        VektorPrimitive circlePrimitive =
            (VektorPrimitive){.kind = VEKTOR_CIRCLE, .circle = *circle};
        VektorStyle style = (VektorStyle){.stroke_color = state->currentColor,
                                          .stroke_width = 0.01};
        vektor_shapebuffer_add_shape(
            state->shapeBuffer, vektor_shape_new(circlePrimitive, style, 0));

        state->selectedShape =
            &(state->shapeBuffer->shapes[state->shapeBuffer->count - 1]);

        vektor_circle_free(circle);

        vektor_circle_set_center(&state->selectedShape->primitive.circle, pos);
        vektor_circle_set_radius(&state->selectedShape->primitive.circle, 0.1f);

        vektor_shapes_update_bbox(state->shapeBuffer);

        vektor_circle_create_handles(&state->selectedShape->primitive.circle,
                                     &state->selectedShape->handles,
                                     &state->selectedShape->handleCount);
    } else if (state->selectedTool == VektorRectangleTool) {

        VektorRectangle* rect = vektor_rectangle_new();
        VektorPrimitive rectPrimitive =
            (VektorPrimitive){.kind = VEKTOR_RECTANGLE, .rectangle = *rect};
        VektorStyle style = (VektorStyle){.stroke_color = state->currentColor,
                                          .stroke_width = 0.01};
        vektor_shapebuffer_add_shape(state->shapeBuffer,
                                     vektor_shape_new(rectPrimitive, style, 0));

        state->selectedShape =
            &(state->shapeBuffer->shapes[state->shapeBuffer->count - 1]);

        vektor_rectangle_free(rect);

        vektor_rectangle_set_start(&state->selectedShape->primitive.rectangle,
                                   pos);
        vektor_rectangle_set_end(&state->selectedShape->primitive.rectangle,
                                 vec2_add(pos, (V2){0.1f, 0.1f}));
        vektor_rectangle_create_handles(
            &state->selectedShape->primitive.rectangle,
            &state->selectedShape->handles, &state->selectedShape->handleCount);

        // state->selectedShape = NULL;
        vektor_shapes_update_bbox(state->shapeBuffer);
    } else if (state->selectedTool == VektorSelectionTool) {
        for (size_t i = 0; i < state->shapeBuffer->count; i++) {
            VektorBBox bbox = vektor_primitive_get_bbox(
                state->shapeBuffer->shapes[i].primitive);
            
            // expand the bbox a little so its not painful to
            // try to grab handles located on the border of said bbox
            bbox = vektor_bbox_expand(bbox, 0.02);

            if (vektor_bbox_isinside(bbox, pos)) {
                state->selectedShape = &(state->shapeBuffer->shapes[i]);
                return;
            }
        }
        // was clicked outside any shapes - reset selection
        vektor_appstate_deselect_shape(state);
    }
}

void vektor_appstate_canvas_drag_begin(GtkGestureDrag* gesture, gdouble x,
                                       gdouble y, gpointer user_data) {
    GtkWidget* widget =
        gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

    VektorAppState* state = (VektorAppState*)user_data;

    int widget_w = gtk_widget_get_width(widget);
    int widget_h = gtk_widget_get_height(widget);

    V2 position =
        (V2){(2 * (x / widget_w)) - 1, 1 - (2 * (y / widget_h))};

    if(state->selectedShape != NULL) {
        VektorShape* selectedShape = state->selectedShape;

        // get selected shape's handles and check
        // if we click any of them
        for(size_t i = 0; i < selectedShape->handleCount; i++) {
            VektorBBox bbox = vektor_shape_get_handle_bbox(selectedShape->handles[i]);
            if(vektor_bbox_isinside(bbox, position)) {
                 // clicked inside handle
                state->heldHandleIndex = i;
                break;
            }
        }

    }
}

void vektor_appstate_canvas_drag_update(GtkGestureDrag* gesture, gdouble x,
                                        gdouble y, gpointer user_data) {

    // ---- setup normalized coordinates (boilerplate) ----
    gdouble start_x, start_y;
    gtk_gesture_drag_get_start_point(gesture, &start_x, &start_y);

    GtkWidget* widget =
        gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

    VektorAppState* state = (VektorAppState*)user_data;

    int widget_w = gtk_widget_get_width(widget);
    int widget_h = gtk_widget_get_height(widget);

    V2 position = (V2){(2 * ((x + start_x) / widget_w)) - 1,
                   1 - (2 * ((y + start_y) / widget_h))};

    // drag handle if selected
    if(state->selectedShape != NULL && state->heldHandleIndex != -1) {
        state->selectedShape->handles[state->heldHandleIndex] = position;
        vektor_shape_handles_updated(state->selectedShape);
    }
}

void vektor_appstate_canvas_drag_end(GtkGestureDrag* gesture, gdouble x,
                                        gdouble y, gpointer user_data) {

    VektorAppState* state = (VektorAppState*)user_data;

    // if we were dragging a handle
    if(state->selectedShape != NULL && state->heldHandleIndex != -1) {
        state->heldHandleIndex = -1; // ...then remove handle drag flag
    }
}

void vektor_appstate_new(VektorWidgetState* wstate, VektorAppState* stateOut) {
    button_tool_set_data* data_linetool = malloc(sizeof(button_tool_set_data));
    data_linetool->state = stateOut;
    data_linetool->tool = VektorLineTool;
    data_linetool->revealer = wstate->workspaceRevealerShapes;

    button_tool_set_data* data_polygontool =
        malloc(sizeof(button_tool_set_data));
    data_polygontool->state = stateOut;
    data_polygontool->tool = VektorPolygonTool;
    data_polygontool->revealer = wstate->workspaceRevealerShapes;

    button_tool_set_data* data_rectangletool =
        malloc(sizeof(button_tool_set_data));
    data_rectangletool->state = stateOut;
    data_rectangletool->tool = VektorRectangleTool;
    data_rectangletool->revealer = wstate->workspaceRevealerShapes;

    button_tool_set_data* data_selecttool =
        malloc(sizeof(button_tool_set_data));
    data_selecttool->state = stateOut;
    data_selecttool->tool = VektorSelectionTool;

    button_tool_set_data* data_circletool =
        malloc(sizeof(button_tool_set_data));
    data_circletool->state = stateOut;
    data_circletool->tool = VektorCircleTool;
    data_circletool->revealer = wstate->workspaceRevealerShapes;

    // populate appstate
    stateOut->startupTime = g_get_monotonic_time();
    stateOut->shapeBuffer = malloc(sizeof(VektorShapeBuffer));
    *stateOut->shapeBuffer = (VektorShapeBuffer){0};
    stateOut->canvas = malloc(sizeof(VektorCanvas));
    stateOut->widgetState = wstate;
    stateOut->currentColor = vektor_color_solid(0, 0, 0);
    stateOut->selectedShape = NULL;
    stateOut->heldHandleIndex = -1;

    VektorCanvasRenderInfo* renderInfo = malloc(sizeof(VektorCanvasRenderInfo));
    renderInfo->zoom = 1;
    renderInfo->panX = 0;
    renderInfo->panY = 0;
    renderInfo->rotation = 0;
    m33_to_gl4(m33_identity(), renderInfo->canvasTransform);
    renderInfo->selectedShape = &(stateOut->selectedShape);
    renderInfo->shapes = stateOut->shapeBuffer;
    renderInfo->startupTime = stateOut->startupTime;
    renderInfo->canvasMat = m33_identity();
    vektor_canvas_init(wstate, stateOut->canvas, renderInfo);
    stateOut->renderInfo = renderInfo;

    // link all the buttons
    g_signal_connect(G_OBJECT(wstate->workspaceButtonLineTool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_linetool);
    g_signal_connect(G_OBJECT(wstate->workspaceButtonRectTool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_rectangletool);
    g_signal_connect(G_OBJECT(wstate->workspaceButtonCircleTool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_circletool);
    g_signal_connect(G_OBJECT(wstate->workspaceButtonPolygonTool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_polygontool);
    g_signal_connect(G_OBJECT(wstate->workspaceButtonSelectionTool), "clicked",
                     G_CALLBACK(appstate_set_tool), data_selecttool);

    // hook subtool revealers to their master buttons
    g_signal_connect(G_OBJECT(wstate->workspaceButtonMasterShapes), "clicked",
                     G_CALLBACK(appstate_reveal_subtools),
                     wstate->workspaceRevealerShapes);

    // hook relevant stuff to master color picker
    g_signal_connect(G_OBJECT(wstate->workspaceColorPicker), "color-changed",
                     G_CALLBACK(appstate_on_color_change), stateOut);

    // hook rgb entries change
    g_signal_connect(G_OBJECT(wstate->sidepanelEntryR), "activate",
                     G_CALLBACK(appstate_on_entry_update),
                     stateOut->widgetState);
    g_signal_connect(G_OBJECT(wstate->sidepanelEntryG), "activate",
                     G_CALLBACK(appstate_on_entry_update),
                     stateOut->widgetState);
    g_signal_connect(G_OBJECT(wstate->sidepanelEntryB), "activate",
                     G_CALLBACK(appstate_on_entry_update),
                     stateOut->widgetState);

    // Add click gesture to canvas
    GtkGesture* canvasClickGesture = gtk_gesture_click_new();
    g_signal_connect(G_OBJECT(canvasClickGesture), "pressed",
                     G_CALLBACK(canvas_onclick), stateOut);
    gtk_widget_add_controller(GTK_WIDGET(wstate->workspaceCanvas),
                              GTK_EVENT_CONTROLLER(canvasClickGesture));

    // Add drag gesture to canvas
    GtkGesture* canvasDragGesture = gtk_gesture_drag_new();
    g_signal_connect(G_OBJECT(canvasDragGesture), "drag-update",
                     G_CALLBACK(vektor_appstate_canvas_drag_update), stateOut);
    g_signal_connect(G_OBJECT(canvasDragGesture), "drag-begin",
                     G_CALLBACK(vektor_appstate_canvas_drag_begin), stateOut);
    g_signal_connect(G_OBJECT(canvasDragGesture), "drag-end",
                     G_CALLBACK(vektor_appstate_canvas_drag_end), stateOut);

    gtk_widget_add_controller(GTK_WIDGET(wstate->workspaceCanvas),
                              GTK_EVENT_CONTROLLER(canvasDragGesture));
}

void vektor_appstate_deselect_shape(VektorAppState* state) {
    state->heldHandleIndex = -1;
    state->selectedShape = NULL;
}