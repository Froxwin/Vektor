#include "gtk/gtk.h"
#include "src/core/primitives.h"
#include "stdio.h"
#include "stdlib.h"

#include "./core/raster.h"
#include "./ui/uicontroller.h"
#include "./ui/vektorcanvas.h"
#include "./util/color.h"

static void on_map(GtkWidget *window, gpointer user_data) {
  vektor_uictrl_map((VektorWidgetState *)user_data);
}

void write_ppm(const char *path, const VektorFramebuffer *fb) {
  FILE *f = fopen(path, "wb");
  if (!f)
	abort();

  fprintf(f, "P6\n%d %d\n255\n", fb->width, fb->height);
  fwrite(fb->pixels, 1, fb->width * fb->height * 4, f);
  fclose(f);
}

static void activate(GtkApplication *app, gpointer user_data) {
  VektorFramebuffer fb = vektor_framebuffer_new(400, 400);

  VektorPolygon triangle = *vektor_polygon_new();
  vektor_polygon_add_point(&triangle, (V2){50, 150});
  vektor_polygon_add_point(&triangle, (V2){200, 180});
  vektor_polygon_add_point(&triangle, (V2){120, 300});

  VektorPolygon star = *vektor_polygon_new();
  vektor_polygon_add_point(&star, (V2){150, 40});
  vektor_polygon_add_point(&star, (V2){180, 110});
  vektor_polygon_add_point(&star, (V2){260, 110});
  vektor_polygon_add_point(&star, (V2){200, 160});
  vektor_polygon_add_point(&star, (V2){220, 240});
  vektor_polygon_add_point(&star, (V2){150, 190});
  vektor_polygon_add_point(&star, (V2){80, 240});
  vektor_polygon_add_point(&star, (V2){100, 160});
  vektor_polygon_add_point(&star, (V2){40, 110});
  vektor_polygon_add_point(&star, (V2){120, 110});

  VektorPrimitiveBuffer prims = {0};
  vektor_primitivebuffer_add_primitive(
      &prims, (VektorPrimitive){.kind = VEKTOR_POLYGON, .polygon = &triangle});
  vektor_primitivebuffer_add_primitive(
      &prims, (VektorPrimitive){.kind = VEKTOR_POLYGON, .polygon = &star});

  rasterize(&fb, &prims);

  VektorWidgetState *widget_state =
	  (VektorWidgetState *)malloc(sizeof(VektorWidgetState));
  vektor_uictrl_init(app, widget_state);

  VektorCanvas *canvas = (VektorCanvas *)malloc(sizeof(VektorCanvas));
  vektor_canvas_init(widget_state, canvas);
  vektor_canvas_fill(canvas, vektor_color_new(255, 0, 0, 255));
  vektor_canvas_drawfrom(&fb, canvas);
  vektor_canvas_update(canvas);

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
