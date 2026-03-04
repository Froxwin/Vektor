#include "gtk/gtk.h"
#include "src/application/applicationstate.h"
#include "src/core/primitives.h"
#include "stdio.h"
#include "stdlib.h"

#include "./core/raster.h"
#include "./ui/uicontroller.h"
#include "./ui/vektorcanvas.h"
#include "./util/color.h"
#include "./application/applicationstate.h"

static void on_map(GtkWidget *window, gpointer user_data) {
  vektor_uictrl_map((VektorWidgetState *)user_data);
}

static void activate(GtkApplication *app, gpointer user_data) {

  VektorWidgetState *widget_state =
	  (VektorWidgetState *)malloc(sizeof(VektorWidgetState));
  vektor_uictrl_init(app, widget_state);
  VektorAppState* app_state = 
		(VektorAppState *)malloc(sizeof(VektorAppState));
	vektor_appstate_new(widget_state, app_state);


  g_signal_connect(widget_state->window, "map", G_CALLBACK(on_map),
				   widget_state);

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
