#include "uicontroller.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "gtk/gtkcssprovider.h"

void vektor_uictrl_init(GtkApplication *app, VektorWidgetState *stateOut) {
  GtkBuilder *builder = gtk_builder_new();
  GError *error = NULL;

  // TODO: .ui files as resources instead of sketchy relative paths
  if (!gtk_builder_add_from_file(builder, "./ui/main.ui", &error)) {
    g_error("Fatal: %s", error->message);
  }

  GtkCssProvider* provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "./ui/main.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(), 
    GTK_STYLE_PROVIDER(provider), 
    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
  );

  stateOut->window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
  stateOut->workspacePaned =
      GTK_PANED(gtk_builder_get_object(builder, "workspace_paned"));
  stateOut->workspaceCanvas =
      GTK_PICTURE(gtk_builder_get_object(builder, "workspace"));

  gtk_window_set_application(stateOut->window, app);
  gtk_window_set_title(stateOut->window, "Vektor");
  gtk_window_set_default_size(stateOut->window, 800, 600);

  g_object_unref(builder);
}

void vektor_uictrl_map(VektorWidgetState *state) {

  // set the workspace divider to 7:3 ratio
  int window_width = gtk_widget_get_width(GTK_WIDGET(state->window));
  g_print("%i", window_width);
  gtk_paned_set_position(state->workspacePaned, 800 * .7);
}
