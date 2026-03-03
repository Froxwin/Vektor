#ifndef VKTR_UICTRL_H
#define VKTR_UICTRL_H

#include "gtk/gtk.h"

/*
Global application widget state, holding references to
all the widgets used in internal logic of the program
*/
typedef struct VektorWidgetState {
    GtkWindow* window;

    GtkPaned* workspacePaned;
    //GtkWidget* Workspace
} VektorWidgetState;

void vektor_uictrl_init(GtkApplication* app, VektorWidgetState* stateOut);
void vektor_uictrl_map(VektorWidgetState* state);

#endif