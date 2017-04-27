/* name: GUI.c
 * author: Candice PERSIL
 * date: 26.04.2017
 * Build: gcc `pkg-config --cflags gtk+-3.0` -o GUI GUI.c `pkg-config --libs gtk+-3.0`
 */

#include <stdlib.h>
#include <gtk/gtk.h>

typedef struct _Label {
	GtkWidget *InputLabel;
	GtkWidget *OutputLabel;
	GtkWidget *PlayLabel;
	GtkWidget *StopLabel;
	GtkWidget *WetLabel;
	GtkWidget *ReflectLabel;
	GtkWidget *DampLabel;
	GtkWidget *Rt60Label;
	GtkWidget *RoomSizeLabel;
	GtkWidget *AreaLabel;
	GtkWidget *VolumeLabel;
} Label;

typedef struct _Widget {
	GtkWidget *WetWidget;
	GtkWidget *ReflectWidget;
	GtkWidget *DampWidget;
	GtkWidget *Rt60Widget;
	GtkWidget *AreaWidget;
	GtkWidget *VolumeWidget;
} Widget;

void user_function (GtkRange *range,
               gpointer  user_data)
{
    printf("Change -> %f\n", gtk_range_get_value(range));
}

int main(int argc, char **argv)
{
	/* Variables */
	GtkWidget * MainWindow = NULL;
	//GtkWidget * InputContainer = NULL;
	Label *lb;
	Widget * Wd;
	GtkWidget * InputAlign = NULL;
	GtkWidget * table = NULL;
	GtkWidget *button_file;
	
	const gchar* sTitle;
	gint sLargeur;
	gint sHauteur;
	gint sPosition_y;
	gint sPosition_x;
	int icone;

	/* Initialisation of GTK+ */
	gtk_init(&argc, &argv);

	/* allocation */
	lb = g_malloc( sizeof(Label) );
	Wd = g_malloc( sizeof(Widget) );

	/* Creation of the window */
	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// Title
	gtk_window_set_title(GTK_WINDOW(MainWindow), "Sound Mixer");
	// Size
	gtk_window_set_default_size(GTK_WINDOW(MainWindow), 580, 460);
	// Position
	gtk_window_set_position(GTK_WINDOW (MainWindow), GTK_WIN_POS_CENTER);
	// Icone
	icone = gtk_window_set_icon_from_file(GTK_WINDOW(MainWindow), "icone.png", NULL);
	// Input label
	lb->InputLabel = gtk_label_new("Input file");
	// Bouton file
	button_file = gtk_file_chooser_button_new (_("Select a song"),GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (button_file),".");
	// Output label
	lb->OutputLabel = gtk_label_new("Output file");
	// Play label
	lb->PlayLabel = gtk_label_new("Play song");
	// Stop label
	lb->StopLabel = gtk_label_new("Stop song");
	// Wet label
	lb->WetLabel = gtk_label_new("Wet");
	// Reflection label
	lb->ReflectLabel = gtk_label_new("Early reflection");
	// Damping label
	lb->DampLabel = gtk_label_new("Damping");
	// Rt60 label
	lb->Rt60Label = gtk_label_new("Rt60");
	// Room Size label
	lb->RoomSizeLabel = gtk_label_new("Room size");
	// Area label
	lb->AreaLabel = gtk_label_new("Area");
	// Volume label
	lb->VolumeLabel = gtk_label_new("Volume");

	// Table
	table = gtk_grid_new();

	//Scales
	Wd->WetWidget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0, 1, 0.01);
	Wd->ReflectWidget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0, 1, 0.01);
	Wd->DampWidget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0, 1, 0.01);
	Wd->Rt60Widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0, 10, 0.1);
	Wd->AreaWidget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0, 10000, 1);
	Wd->VolumeWidget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0, 200000, 1);
	gtk_range_set_inverted(Wd->WetWidget, TRUE);
	gtk_range_set_inverted(Wd->ReflectWidget, TRUE);
	gtk_range_set_inverted(Wd->DampWidget, TRUE);
	gtk_range_set_inverted(Wd->Rt60Widget, TRUE);
	gtk_range_set_inverted(Wd->AreaWidget, TRUE);
	gtk_range_set_inverted(Wd->VolumeWidget, TRUE);

	/* alignment */
	//InputAlign = gtk_alignment_new(0.5, 0.5, 0, 0);


	g_signal_connect(G_OBJECT(MainWindow), "delete-event",
						G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect (G_OBJECT(Wd->WetWidget), "value-changed", G_CALLBACK (user_function), NULL);

	gtk_container_set_border_width (GTK_CONTAINER(MainWindow), 20);

	/* Create an EventBox and add it to our toplevel window */
	//InputContainer = gtk_event_box_new ();
	//gtk_container_add (GTK_CONTAINER(MainWindow), InputContainer);

	// add table in Window
	gtk_container_add(GTK_CONTAINER(MainWindow), GTK_WIDGET(table));

	// Widgets
	gtk_scale_get_layout (GTK_SCALE(Wd->WetWidget));
	gtk_scale_get_layout (GTK_SCALE(Wd->ReflectWidget));
	gtk_scale_get_layout (GTK_SCALE(Wd->DampWidget));
	gtk_scale_get_layout (GTK_SCALE(Wd->Rt60Widget));
	gtk_scale_get_layout (GTK_SCALE(Wd->AreaWidget));
	gtk_scale_get_layout (GTK_SCALE(Wd->VolumeWidget));
	
	/* Table */
	// GtkGrid *grid,GtkWidget *child,gint left,gint top,gint width,gint height
	gtk_grid_attach(GTK_GRID(table), lb->InputLabel, 0, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(table), lb->OutputLabel, 2, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(table), lb->PlayLabel, 1, 3, 2, 1);
	gtk_grid_attach(GTK_GRID(table), lb->StopLabel, 2, 3, 2, 1);
	gtk_grid_attach(GTK_GRID(table), lb->WetLabel, 0, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->ReflectLabel, 1, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->DampLabel, 2, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->Rt60Label, 3, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->RoomSizeLabel, 4, 4, 2, 1);
	gtk_grid_attach(GTK_GRID(table), lb->AreaLabel, 4, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->VolumeLabel, 5, 5, 1, 1);


	gtk_grid_attach(GTK_GRID(table), Wd->WetWidget, 0, 5, 1, 4);
	gtk_grid_attach(GTK_GRID(table), Wd->ReflectWidget, 1, 5, 1, 4);
	gtk_grid_attach(GTK_GRID(table), Wd->DampWidget, 2, 5, 1, 4);
	gtk_grid_attach(GTK_GRID(table), Wd->Rt60Widget, 3, 5, 1, 4);
	gtk_grid_attach(GTK_GRID(table), Wd->AreaWidget, 4, 6, 1, 4);
	gtk_grid_attach(GTK_GRID(table), Wd->VolumeWidget, 5, 6, 1, 4);
	
	gtk_grid_attach(GTK_GRID(table), bouton_file, 0, 1, 2, 1);
	
	// rows
	gtk_grid_set_row_homogeneous (GTK_GRID(table), FALSE);
	gtk_grid_set_row_spacing (GTK_GRID(table), 40);
	// columms
	gtk_grid_set_column_homogeneous (GTK_GRID(table), FALSE);
	gtk_grid_set_column_spacing (GTK_GRID(table), 100);

	/* Get information for the window */
	// Title
	sTitle = gtk_window_get_title(GTK_WINDOW(MainWindow));
	// Size
	gtk_window_get_default_size(GTK_WINDOW(MainWindow), &sLargeur, &sHauteur);
	// Position
	gtk_window_get_position(GTK_WINDOW(MainWindow), &sPosition_x, &sPosition_y);
	// Rows
	gtk_grid_get_row_homogeneous (GTK_GRID(table));
	gtk_grid_get_row_spacing (GTK_GRID(table));
	// Column
	gtk_grid_get_column_homogeneous (GTK_GRID(table));
	gtk_grid_get_column_spacing (GTK_GRID(table));

	// Input Label
	//gtk_container_add (GTK_CONTAINER (InputContainer), lb->InputLabel);

	/* Display and event loops */
	printf("\n-------\nWindow\n------- \n \nTitle: %s \nSize: %d x %d \nPosition: %dy %dx\nIcone: ", sTitle, sLargeur, sHauteur, sPosition_y, sPosition_x);

	gtk_widget_show_all(MainWindow);
	gtk_main();

	return EXIT_SUCCESS;
}
