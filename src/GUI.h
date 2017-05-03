//
// Created by Candice Persil on 02/05/2017.
//

#ifndef GUI_H
#define GUI_H

#include <stdlib.h>
#include <gtk/gtk.h>
#include "main.h"
#include "delay_stuff.h"

gdouble get_wet (GtkRange *range, gpointer  user_data);

gdouble get_reflection (GtkRange *range, gpointer  user_data);

gdouble get_damping (GtkRange *range, gpointer  user_data);

gdouble get_rt60 (GtkRange *range, gpointer  user_data);

gdouble get_area (GtkRange *range, gpointer  user_data);

gdouble get_volume (GtkRange *range, gpointer  user_data);

gboolean play_button (GtkWidget *widget, gpointer user_data);
gboolean pause_button (GtkWidget *widget, gpointer user_data);

const gchar * enter_callback( GtkWidget *widget, GtkWidget *entry );

void init_gui(GtkWidget *MainWindow);


#endif //GUI_H
