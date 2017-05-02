//
// Created by Candice Persil on 02/05/2017.
//

#ifndef GUI_H
#define GUI_H

#include <stdlib.h>
#include <gtk/gtk.h>


GtkRange * get_wet (GtkRange *range, gpointer  user_data);

GtkRange * get_reflection (GtkRange *range, gpointer  user_data);

GtkRange * get_damping (GtkRange *range, gpointer  user_data);

GtkRange * get_rt60 (GtkRange *range, gpointer  user_data);

GtkRange * get_area (GtkRange *range, gpointer  user_data);

GtkRange * get_volume (GtkRange *range, gpointer  user_data);

gboolean play_button (GtkWidget *widget, gpointer user_data);
gboolean pause_button (GtkWidget *widget, gpointer user_data);

static void enter_callback( GtkWidget *widget, GtkWidget *entry );



#endif //GUI_H
