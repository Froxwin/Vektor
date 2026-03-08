#include "colorwheel.h"
#include "cairo.h"
#include "gtk/gtk.h"
#include "gtk/gtkshortcut.h"

#define M_PI 3.14159265358979323846

enum { COLOR_CHANGED, LAST_SIGNAL };
static guint signals[LAST_SIGNAL];

struct _VektorColorWheel {
    GtkDrawingArea parent_instance;

    double hue;
    double saturation;
    double lightness;

    gboolean dragging_wheel;
    gboolean dragging_triangle;
};

G_DEFINE_TYPE(VektorColorWheel, vektor_color_wheel, GTK_TYPE_DRAWING_AREA)

static gboolean point_in_triangle(
    double px, double py,
    double ax, double ay,
    double bx, double by,
    double cx, double cy,
    double* u, double* v, double* w)
{
    double denom =
        (by - cy)*(ax - cx) +
        (cx - bx)*(ay - cy);

    *u =
        ((by - cy)*(px - cx) +
         (cx - bx)*(py - cy)) / denom;

    *v =
        ((cy - ay)*(px - cx) +
         (ax - cx)*(py - cy)) / denom;

    *w = 1 - *u - *v;

    return (*u >= 0 && *v >= 0 && *w >= 0);
}

static void closest_point_on_segment(
    double px, double py,
    double ax, double ay,
    double bx, double by,
    double *rx, double *ry)
{
    double abx = bx - ax;
    double aby = by - ay;

    double apx = px - ax;
    double apy = py - ay;

    double t = (apx*abx + apy*aby) / (abx*abx + aby*aby);

    if (t < 0) t = 0;
    if (t > 1) t = 1;

    *rx = ax + abx * t;
    *ry = ay + aby * t;
}

static void closest_point_on_triangle(
    double px, double py,
    double ax, double ay,
    double bx, double by,
    double cx, double cy,
    double *rx, double *ry)
{
    double p1x,p1y;
    double p2x,p2y;
    double p3x,p3y;

    closest_point_on_segment(px,py, ax,ay, bx,by, &p1x,&p1y);
    closest_point_on_segment(px,py, bx,by, cx,cy, &p2x,&p2y);
    closest_point_on_segment(px,py, cx,cy, ax,ay, &p3x,&p3y);

    double d1 = (px-p1x)*(px-p1x) + (py-p1y)*(py-p1y);
    double d2 = (px-p2x)*(px-p2x) + (py-p2y)*(py-p2y);
    double d3 = (px-p3x)*(px-p3x) + (py-p3y)*(py-p3y);

    if (d1 <= d2 && d1 <= d3) { *rx = p1x; *ry = p1y; }
    else if (d2 <= d3) { *rx = p2x; *ry = p2y; }
    else { *rx = p3x; *ry = p3y; }
}

static void vektor_color_wheel_snapshot(GtkWidget* widget, GtkSnapshot* snapshot) {
    VektorColorWheel* self = VEKTOR_COLOR_WHEEL(widget);

    int width = gtk_widget_get_width(widget);
    int height = gtk_widget_get_height(widget);

    graphene_rect_t bounds = GRAPHENE_RECT_INIT(0,0,width,height);
    cairo_t* cr = gtk_snapshot_append_cairo(snapshot, &bounds);

    double cx = width / 2.0;
    double cy = height / 2.0;

    double outer_radius = MIN(width, height) / 2.0;
    double wheel_radius = outer_radius * 0.95;
    double inner_radius = wheel_radius * 0.9;

    double triangle_radius = wheel_radius * 0.75;

    // wheel draw
    for(int a = 0; a < 360; a++) {
        double angle_1 = a*(M_PI / 180.0);
        double angle_2 = (a + 1) * (M_PI / 180.0);

        cairo_new_path(cr);
        cairo_arc(cr, cx, cy, wheel_radius, angle_1, angle_2);
        cairo_arc_negative(cr, cx, cy, inner_radius, angle_2, angle_1);
        cairo_close_path(cr);

        float r,g,b;
        gtk_hsv_to_rgb(a / 360.0, 1.0, 1.0, &r, &g, &b);

        cairo_set_source_rgb(cr, r, g, b);
        cairo_fill(cr);
    }

    // triangle draw
    double ax = cx +  triangle_radius;
    double ay = cy;

    double bx = cx - 0.5 * triangle_radius;
    double by = cy + 0.866 * triangle_radius;

    double cx2 = cx - 0.5 * triangle_radius;
    double cy2 = cy - 0.866 * triangle_radius;

    cairo_new_path(cr);

    cairo_move_to(cr, ax, ay);
    cairo_line_to(cr, bx, by);
    cairo_line_to(cr, cx2, cy2);
    cairo_close_path(cr);

    cairo_save(cr);
    cairo_clip(cr);

    // base color (pure hue at full brightness)
    float r, g, b;
    gtk_hsv_to_rgb(self->hue, 1.0, 1.0, &r, &g, &b);
    cairo_set_source_rgb(cr, r, g, b);
    cairo_paint(cr);

    // White gradient: from pure hue (right) → white (bottom)
    cairo_pattern_t* white = cairo_pattern_create_linear(ax, ay, bx, by);
    cairo_pattern_add_color_stop_rgba(white, 0.0, 1,1,1, 0.0);     // transparent at pure hue
    cairo_pattern_add_color_stop_rgba(white, 1.0, 1,1,1, 1.0);     // opaque white at bottom
    cairo_set_source(cr, white);
    cairo_paint(cr);
    cairo_pattern_destroy(white);

    // Black gradient: from pure hue (right) → black (top)
    cairo_pattern_t* black = cairo_pattern_create_linear(ax, ay, cx2, cy2);
    cairo_pattern_add_color_stop_rgba(black, 0.0, 0,0,0, 0.0);     // transparent at pure hue
    cairo_pattern_add_color_stop_rgba(black, 1.0, 0,0,0, 1.0);     // opaque black at top
    cairo_set_source(cr, black);
    cairo_paint(cr);
    cairo_pattern_destroy(black);

    cairo_restore(cr);

    // triangle outline
    cairo_set_source_rgb(cr,.1,.1,.1);
    cairo_move_to(cr, ax, ay);
    cairo_line_to(cr, bx, by);
    cairo_line_to(cr, cx2, cy2);
    cairo_close_path(cr);

    cairo_set_source_rgb(cr,.1,.1,.1);
    cairo_stroke(cr);

    // selectors draw

    // triangle selector
    double chroma_weight = self->saturation * self->lightness;
    double white_weight  = (1.0 - self->saturation) * self->lightness;
    double black_weight  = 1.0 - self->lightness;

    double px = ax * chroma_weight + bx * white_weight + cx2 * black_weight;
    double py = ay * chroma_weight + by * white_weight + cy2 * black_weight;

    cairo_arc(cr, px, py, 5, 0, 2*M_PI);

    float fr, fg, fb;
    vektor_color_wheel_get_colorout(self, &fr, &fg, &fb);
    cairo_set_source_rgb(cr, fr, fg, fb);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
    cairo_set_line_width(cr, 2.0);
    cairo_stroke(cr);

    // wheel selector
    double selector_angle = self->hue * 2 * M_PI;
    double selector_width = 0.08;

    cairo_new_path(cr);

    cairo_arc(cr,
            cx, cy,
            wheel_radius,
            selector_angle - selector_width,
            selector_angle + selector_width);

    cairo_arc_negative(cr,
                    cx, cy,
                    inner_radius,
                    selector_angle + selector_width,
                    selector_angle - selector_width);

    cairo_close_path(cr);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_stroke(cr);

    cairo_destroy(cr);
}

static void on_click(GtkGestureClick* gesture, int n_press, double x, double y, gpointer data) {
    VektorColorWheel* wheel = VEKTOR_COLOR_WHEEL(data);
    GtkWidget* widget = GTK_WIDGET(wheel);

    int width = gtk_widget_get_width(widget);
    int height = gtk_widget_get_height(widget);

    double outer_radius = MIN(width, height) / 2.0;
    double wheel_radius = outer_radius * 0.95;
    double inner_radius = wheel_radius * 0.9;

    double triangle_radius = wheel_radius * 0.75;

    double cx = width / 2.0;
    double cy = height / 2.0;

    double ax = cx +  triangle_radius;
    double ay = cy;

    double bx = cx - 0.5 * triangle_radius;
    double by = cy + 0.866 * triangle_radius;

    double cx2 = cx - 0.5 * triangle_radius;
    double cy2 = cy - 0.866 * triangle_radius;

    double u,v,w;
    gboolean inside = point_in_triangle(x,y, ax,ay, bx,by, cx2,cy2, &u,&v,&w);
    if(inside) { // pick point in the triangle

        double denom = u + v;
        if (denom > 0.0001) {  // avoid div-by-zero at black vertex
            wheel->saturation = u / denom;
        } else {
            wheel->saturation = 0.0;  // arbitrary, since S irrelevant at V=0
        }
        wheel->lightness = denom;
        g_signal_emit(wheel, signals[COLOR_CHANGED], 0);

    } else {
        double dx = x - cx;
        double dy = y - cy;
        double dist = sqrt(dx*dx+dy*dy);

        if(dist > inner_radius && dist < outer_radius) { // pick point on color wheel
            
            double angle = atan2(dy, dx);
            if(angle < 0) { angle += 2 * M_PI; }

            wheel->hue = angle / (2*M_PI);
            g_signal_emit(wheel, signals[COLOR_CHANGED], 0);
            
        } else if (dist < inner_radius) { // snap to triangle edge
            double sx,sy;

            closest_point_on_triangle(
                x,y,
                ax,ay,
                bx,by,
                cx2,cy2,
                &sx,&sy
            );

            x = sx;
            y = sy;

            point_in_triangle(x,y, ax,ay, bx,by, cx2,cy2, &u,&v,&w);

            // calculate triangle point
            double denom = u + v;
            if (denom > 0.0001) {
                wheel->saturation = u / denom;
            } else {
                wheel->saturation = 0.0;
            }
            wheel->lightness = denom;
            g_signal_emit(wheel, signals[COLOR_CHANGED], 0);
        }
        
    }

    gtk_widget_queue_draw(widget);
}

static void on_drag(GtkGestureDrag* gesture, double offset_x, double offset_y, gpointer data) {
    double x,y;
    gtk_gesture_drag_get_start_point(gesture,&x,&y);

    x += offset_x;
    y += offset_y;

    on_click(NULL,0,x,y,data);
}

static void vektor_color_wheel_init(VektorColorWheel* self) {
    GtkGesture* click = gtk_gesture_click_new();
    gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(click));

    g_signal_connect(click, "pressed", G_CALLBACK(on_click), self);

    GtkGesture* drag = gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(drag));

    g_signal_connect(drag, "drag-update", G_CALLBACK(on_drag), self);
}

static void vektor_color_wheel_class_init(VektorColorWheelClass* klass) {
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(klass);
    widget_class->snapshot = vektor_color_wheel_snapshot;

    signals[COLOR_CHANGED] =
    g_signal_new(
        "color-changed",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0,
        NULL,
        NULL,
        NULL,
        G_TYPE_NONE,
        0
    );
}

GtkWidget* vektor_color_wheel_new(void) {
    return g_object_new(VEKTOR_TYPE_COLOR_WHEEL, NULL);
}

VektorColor vektor_color_wheel_get_color(VektorColorWheel* wheel) {
    float r,g,b;
    gtk_hsv_to_rgb(wheel->hue,
                   wheel->saturation,
                   wheel->lightness,
                   &r, &g, &b);
    
    return (VektorColor) {
        .r = (unsigned char)(r*255), 
        .g = (unsigned char)(g*255), 
        .b = (unsigned char)(b*255) 
    };
}

void vektor_color_wheel_get_colorout(VektorColorWheel* wheel, float* r, float* g, float* b) {
    gtk_hsv_to_rgb(wheel->hue,
                   wheel->saturation,
                   wheel->lightness,
                   r, g, b);
}

void vektor_color_wheel_set_color(VektorColorWheel* wheel, VektorColor c) {
    float h,s,v;
    gtk_rgb_to_hsv(c.r/255.0, c.g/255.0, c.b/255.0, &h, &s, &v);
    wheel->hue = (float)h;
    wheel->saturation = (float)s;
    wheel->lightness = (float)v;

    gtk_widget_queue_draw(GTK_WIDGET(wheel));
    g_signal_emit(wheel, signals[COLOR_CHANGED], 0);
}