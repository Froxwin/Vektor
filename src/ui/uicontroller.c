#include "uicontroller.h"
#include "gtk/gtk.h"

void vektor_uictrl_init(GtkApplication* app, VektorWidgetState* stateOut) {
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;

    g_print("%s\n", g_get_current_dir());

    // TODO: .ui files as resources instead of sketchy relative paths
    if(!gtk_builder_add_from_file(builder, "./ui/main.ui", &error)) {
        g_error("Fatal: %s", error->message);
    }

    stateOut->window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    stateOut->workspacePaned = GTK_PANED(gtk_builder_get_object(builder, "workspace_paned"));

    gtk_window_set_application(stateOut->window, app);
    gtk_window_set_title(stateOut->window, "Vektor");
    gtk_window_set_default_size(stateOut->window, 800, 600);

    g_object_unref(builder);
}

void vektor_uictrl_map(VektorWidgetState* state) {

    // set the workspace divider to 7:3 ratio
    int window_width = gtk_widget_get_width(GTK_WIDGET(state->window));
    g_print("%i", window_width);
    gtk_paned_set_position(state->workspacePaned, 800 * .7);
}
