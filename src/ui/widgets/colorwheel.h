#ifndef VKTR_COLORWHEEL_H
#define VKTR_COLORWHEEL_H

#include "gtk/gtk.h"
#include "src/util/color.h"

#define VEKTOR_TYPE_COLOR_WHEEL vektor_color_wheel_get_type()
G_DECLARE_FINAL_TYPE(VektorColorWheel, vektor_color_wheel, VEKTOR, COLOR_WHEEL, GtkDrawingArea)

GtkWidget* vektor_color_wheel_new(void);
VektorColor vektor_color_wheel_get_color(VektorColorWheel* wheel);
void vektor_colorout_wheel_get_color(VektorColorWheel* wheel, float* r, float* g, float* b);

#endif