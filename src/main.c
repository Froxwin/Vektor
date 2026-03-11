#include "glib.h"
#include "gtk/gtk.h"
#include "src/application/applicationstate.h"
#include "stdio.h"
#include "stdlib.h"

#include "./application/applicationstate.h"
#include "./ui/uicontroller.h"


static int update_callback(gpointer data) {
    VektorAppState* appstate = (VektorAppState*)data;
    gtk_gl_area_queue_render(
        GTK_GL_AREA(appstate->widgetState->workspaceCanvas));
    return G_SOURCE_CONTINUE;
}

static void activate(GtkApplication* app, gpointer user_data) {

    VektorWidgetState* widget_state =
        (VektorWidgetState*)malloc(sizeof(VektorWidgetState));
    vektor_uictrl_init(app, widget_state);
    VektorAppState* app_state = (VektorAppState*)malloc(sizeof(VektorAppState));
    vektor_appstate_new(widget_state, app_state);

    g_timeout_add(1, update_callback, app_state);

    gtk_window_present(widget_state->window);
}

int main(int argc, char** argv) {

    GtkApplication* app;
    int status;

    app = gtk_application_new("dev.frox.vektor", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
