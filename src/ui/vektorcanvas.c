#include "gtk/gtk.h"

#include "../core/raster.h"
#include "uicontroller.h"
#include "vektorcanvas.h"

#define VKTR_CANVAS_WIDTH 400
#define VKTR_CANVAS_HEIGHT 400
#define VKTR_CANVAS_SIZE (VKTR_CANVAS_WIDTH * VKTR_CANVAS_HEIGHT * 4)

void vektor_canvas_init(VektorWidgetState *state, VektorCanvas *canvasOut) {
    canvasOut->canvasWidget = state->workspaceCanvas;
    canvasOut->width = VKTR_CANVAS_WIDTH;
    canvasOut->height = VKTR_CANVAS_HEIGHT;
    canvasOut->canvasPixels = g_malloc0(VKTR_CANVAS_SIZE);

    canvasOut->canvasPixelBytes =
        g_bytes_new(canvasOut->canvasPixels, VKTR_CANVAS_SIZE);
    canvasOut->canvasTexture = gdk_memory_texture_new(
        VKTR_CANVAS_WIDTH, VKTR_CANVAS_HEIGHT, GDK_MEMORY_R8G8B8A8,
        canvasOut->canvasPixelBytes, VKTR_CANVAS_WIDTH * 4);

    gtk_picture_set_paintable(canvasOut->canvasWidget,
                              GDK_PAINTABLE(canvasOut->canvasTexture));
    gtk_picture_set_content_fit(GTK_PICTURE(canvasOut->canvasWidget),
                                GTK_CONTENT_FIT_CONTAIN);
    // g_object_unref(bytes);
}

/* Generate new texture based on canvasPixels*/
void vektor_canvas_update(VektorCanvas *canvas) {
    g_bytes_unref(canvas->canvasPixelBytes);
    canvas->canvasPixelBytes =
        g_bytes_new(canvas->canvasPixels, VKTR_CANVAS_SIZE);

    g_object_unref(canvas->canvasTexture);
    canvas->canvasTexture = gdk_memory_texture_new(
        canvas->width, canvas->height, GDK_MEMORY_R8G8B8A8,
        canvas->canvasPixelBytes, canvas->width * 4);

    gtk_picture_set_paintable(canvas->canvasWidget,
                              GDK_PAINTABLE(canvas->canvasTexture));
}

void vektor_canvas_fill(VektorCanvas *canvas, VektorColor color) {
    for (int x = 0; x < VKTR_CANVAS_WIDTH; x++) {
        for (int y = 0; y < VKTR_CANVAS_HEIGHT; y++) {
            int i = (y * VKTR_CANVAS_WIDTH + x) * 4;
            canvas->canvasPixels[i + 0] = color.r;
            canvas->canvasPixels[i + 1] = color.g;
            canvas->canvasPixels[i + 2] = color.b;
            canvas->canvasPixels[i + 3] = color.a;
        }
    }
}

void vektor_canvas_drawfrom(VektorFramebuffer *fb, VektorCanvas *target) {
    for (int x = 0; x < fb->width; x++) {
        for (int y = 0; y < fb->height; y++) {

            int i = (y * fb->width + x) * 4;
            target->canvasPixels[i + 0] = (guchar)fb->pixels[i + 0];
            target->canvasPixels[i + 1] = (guchar)fb->pixels[i + 1];
            target->canvasPixels[i + 2] = (guchar)fb->pixels[i + 2];
            target->canvasPixels[i + 3] = (guchar)fb->pixels[i + 3];
        }
    }
}