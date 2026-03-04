#include "gtk/gtk.h"
#include "stdio.h"
#include "stdlib.h"

#include "./core/raster.h"
#include "./ui/uicontroller.h"
#include "./ui/vektorcanvas.h"

static void on_map(GtkWidget *window, gpointer user_data) {
  vektor_uictrl_map((VektorWidgetState *)user_data);
}

static void activate(GtkApplication *app, gpointer user_data) {

  VektorWidgetState *widget_state =
      (VektorWidgetState *)malloc(sizeof(VektorWidgetState));
  vektor_uictrl_init(app, widget_state);

  VektorCanvas *canvas = (VektorCanvas *)malloc(sizeof(VektorCanvas));
  vektor_canvas_init(widget_state, canvas);
  VektorCanvasColor red = vektor_color_new(255, 0, 0, 255);
  vektor_canvas_fill(canvas, &red);
  vektor_canvas_update(canvas);

  g_signal_connect(widget_state->window, "map", G_CALLBACK(on_map),
                   widget_state);

  gtk_window_present(widget_state->window);
}

void write_ppm(const char *path, const Framebuffer *fb) {
  FILE *f = fopen(path, "wb");
  if (!f)
    abort();

  fprintf(f, "P6\n%d %d\n255\n", fb->width, fb->height);
  fwrite(fb->pixels, 1, fb->width * fb->height * 3, f);
  fclose(f);
}

int main(int argc, char **argv) {
  Framebuffer fb = mk_framebuffer(256, 256);
  EdgeBuffer edges = {0};

  Polygon pg = *mk_polygon();
  add_point_polygon(&pg, (V2){50, 50});
  add_point_polygon(&pg, (V2){200, 80});
  add_point_polygon(&pg, (V2){120, 200});

  flatten_polygon(&edges, &pg);

  for (size_t i = 0; i < edges.count; i++) {
    draw_line(&fb, edges.edges[i].p1, edges.edges[i].p2, 255, 255, 255);
  }

  write_ppm("out.ppm", &fb);

  GtkApplication *app;
  int status;

  app = gtk_application_new("dev.frox.vektor", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
