#include "uicontroller.h"
#include "gdk/gdk.h"
#include "glib-object.h"
#include "gtk/gtk.h"
#include "gtk/gtkcssprovider.h"
#include "gtk/gtkrevealer.h"
#include "src/ui/widgets/colorwheel.h"

void vektor_uictrl_init(GtkApplication* app, VektorWidgetState* stateOut) {
    g_type_ensure(VEKTOR_TYPE_COLOR_WHEEL);

    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;

    // TODO: .ui files as resources instead of sketchy relative paths
    if (!gtk_builder_add_from_file(builder, "./ui/main.ui", &error)) {
        g_error("Fatal: %s", error->message);
    }

    // Load css
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./ui/main.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Load theme
    gtk_icon_theme_add_search_path(
        gtk_icon_theme_get_for_display(gdk_display_get_default()), "icons");

    GtkIconTheme* theme =
        gtk_icon_theme_get_for_display(gdk_display_get_default());

    /*if (gtk_icon_theme_has_icon(theme, "vektor-circle-symbolic"))
        g_print("GTK sees it!\n");
    else
        g_print("Still invisible...\n");*/

    // populate state
    stateOut->window =
        GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    stateOut->workspacePaned =
        GTK_PANED(gtk_builder_get_object(builder, "workspace_paned"));
    stateOut->sidepanelPaned =
        GTK_PANED(gtk_builder_get_object(builder, "sidepanel"));
    stateOut->workspaceCanvas =
        GTK_GL_AREA(gtk_builder_get_object(builder, "workspace"));

    stateOut->workspaceButtonMasterShapes =
        GTK_BUTTON(gtk_builder_get_object(builder, "button_shapetools"));
    stateOut->workspaceRevealerShapes =
        GTK_REVEALER(gtk_builder_get_object(builder, "shape_revealer"));
    stateOut->workspaceButtonLineTool =
        GTK_BUTTON(gtk_builder_get_object(builder, "button_linetool"));
    stateOut->workspaceButtonRectTool =
        GTK_BUTTON(gtk_builder_get_object(builder, "button_rectangletool"));
    stateOut->workspaceButtonCircleTool =
        GTK_BUTTON(gtk_builder_get_object(builder, "button_circletool"));
    stateOut->workspaceButtonPolygonTool =
        GTK_BUTTON(gtk_builder_get_object(builder, "button_polygontool"));
    stateOut->workspaceButtonSelectionTool =
        GTK_BUTTON(gtk_builder_get_object(builder, "button_selecttool"));
    stateOut->workspaceColorPicker =
        VEKTOR_COLOR_WHEEL(gtk_builder_get_object(builder, "color_picker"));

    stateOut->sidepanelEntryR =
        GTK_ENTRY(gtk_builder_get_object(builder, "spin_color_r"));
    stateOut->sidepanelEntryG =
        GTK_ENTRY(gtk_builder_get_object(builder, "spin_color_g"));
    stateOut->sidepanelEntryB =
        GTK_ENTRY(gtk_builder_get_object(builder, "spin_color_b"));

    // Set window properties
    gtk_window_set_application(stateOut->window, app);
    gtk_window_set_title(stateOut->window, "Vektor");
    gtk_window_set_default_size(stateOut->window, 800, 600);

    g_object_unref(builder);
}

void vektor_uictrl_map(VektorWidgetState* state) {
    gtk_paned_set_position(state->workspacePaned, 800 * .7);
    gtk_paned_set_position(state->sidepanelPaned, 250);
}
