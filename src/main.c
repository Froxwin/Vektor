#include "gtk/gtk.h"
#include "stdio.h"
#include "stdlib.h"

#include "./ui/uicontroller.h"
#include "./ui/vektorcanvas.h"


static void on_map(GtkWidget* window, gpointer user_data) {
  vektor_uictrl_map((VektorWidgetState*)user_data);
}

static void activate(GtkApplication *app, gpointer user_data) {

  VektorWidgetState* widget_state = (VektorWidgetState*)malloc(sizeof(VektorWidgetState));
  vektor_uictrl_init(app, widget_state);

  VektorCanvas* canvas = (VektorCanvas*)malloc(sizeof(VektorCanvas));
  vektor_canvas_init(widget_state, canvas);
  VektorCanvasColor red = vektor_color_new(255, 0, 0, 255);
  vektor_canvas_fill(canvas, &red);
  vektor_canvas_update(canvas);

  g_signal_connect(widget_state->window, "map", G_CALLBACK(on_map), widget_state);

  gtk_window_present(widget_state->window);
}

int main(int argc, char **argv) {

  GtkApplication *app;
  int status;

  app = gtk_application_new("dev.frox.vektor", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
