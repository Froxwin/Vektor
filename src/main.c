#include "gtk/gtk.h"
#include "stdio.h"

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Vektor");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
  printf("\x1b[1;46m IGNITION \x1b[0m\n");

  GtkApplication *app;
  int status;

  app = gtk_application_new("dev.frox.vektor", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
