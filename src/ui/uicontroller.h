#ifndef VKTR_UICTRL_H
#define VKTR_UICTRL_H

#include "gtk/gtk.h"
#include "gtk/gtkrevealer.h"
#include "src/ui/widgets/colorwheel.h"

/*
Global application widget state, holding references to
all the widgets used in internal logic of the program
*/
typedef struct VektorWidgetState {
    GtkWindow* window;
    GtkPaned* workspacePaned;
    GtkPaned* sidepanelPaned;
    GtkGLArea* workspaceCanvas;

    GtkButton* workspaceButtonMasterShapes;
    GtkRevealer* workspaceRevealerShapes;
    GtkButton* workspaceButtonLineTool;
    GtkButton* workspaceButtonRectTool;
    GtkButton* workspaceButtonCircleTool;
    GtkButton* workspaceButtonPolygonTool;
    GtkButton* workspaceButtonSelectionTool;

    VektorColorWheel* workspaceColorPicker;

    GtkEntry* sidepanelEntryR;
    GtkEntry* sidepanelEntryG;
    GtkEntry* sidepanelEntryB;

    // GtkWidget* Workspace
} VektorWidgetState;

void vektor_uictrl_init(GtkApplication* app, VektorWidgetState* stateOut);
void vektor_uictrl_map(VektorWidgetState* state);

#endif