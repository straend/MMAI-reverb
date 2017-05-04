/* name: GUI.c
 * author: Candice PERSIL
 * date: 26.04.2017
 * Build: gcc `pkg-config --cflags gtk+-3.0` -o GUI GUI.c `pkg-config --libs gtk+-3.0`
 */

#include "GUI.h"

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
	GtkWidget *button_file;
	GtkWidget *OutputWidget;
	GtkWidget *PlayButton;
	GtkWidget *PauseButton;
} Widget;
Widget * Wd;

gdouble get_wet (GtkRange *range,
                reverb_settings_s *rs)
{
    printf("Wet -> %f\n", gtk_range_get_value(range));
    set_wetmix(gtk_range_get_value(range));
    return gtk_range_get_value(range);
}

gdouble get_reflection (GtkRange *range,
               gpointer  user_data)
{
    printf("Reflection -> %f\n", gtk_range_get_value(range));
    set_earlyRD(gtk_range_get_value(range));
    return gtk_range_get_value(range);
}

gdouble get_damping (GtkRange *range,
               gpointer  user_data)
{
    printf("Damping -> %f\n", gtk_range_get_value(range));
    set_damping(gtk_range_get_value(range));
    return gtk_range_get_value(range);
}

gdouble get_rt60 (GtkRange *range,
               gpointer  user_data)
{
    printf("Rt60 -> %f\n", gtk_range_get_value(range));
    set_rt60(gtk_range_get_value(range));
    return gtk_range_get_value(range);
}

gdouble get_area (GtkRange *range,
                reverb_settings_s *rs)
{
    printf("Area -> %f\n", gtk_range_get_value(range));
    float area = gtk_range_get_value(range);
    float volume = gtk_range_get_value(GTK_RANGE(Wd->VolumeWidget));
    gtk_range_set_value(GTK_RANGE(Wd->Rt60Widget), get_rt60_from_volume_area(volume, area));
    return gtk_range_get_value(range);
}

gdouble get_volume (GtkRange *range,
                reverb_settings_s *rs)
{
    printf("Volume -> %f\n", gtk_range_get_value(range));
    //rs->volume = gtk_range_get_value(range);
    float volume = gtk_range_get_value(range);
    float area = gtk_range_get_value(GTK_RANGE(Wd->AreaWidget));
    gtk_range_set_value(GTK_RANGE(Wd->Rt60Widget), get_rt60_from_volume_area(volume, area));
    return gtk_range_get_value(range);
}

gboolean play_button (GtkWidget *widget, gpointer user_data)
{
    gboolean bplay = FALSE;
    bplay = TRUE;
    g_print ("play was pressed, bplay = %d\n", bplay);
    start_playback();
    return bplay;
}

gboolean pause_button (GtkWidget *widget, gpointer user_data)
{
    static gboolean bpause = FALSE;
    g_print ("pause was pressed, bpause = %d\n", bpause);

    pause_playback(bpause);
    bpause = !bpause;
    return bpause;
}

const gchar * enter_callback( GtkWidget *widget, GtkWidget *entry )
{
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("New file name entered: %s\n", entry_text);
  //entry_text countains the new file name
  return entry_text;
}
void file_selected_callback (GtkFileChooserButton *widget, gpointer user_data)
{
  char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
  file_selected(fname);
}

void init_gui(GtkWidget *MainWindow, reverb_settings_s *rs)
{
	/* Variables */
	Label *lb;
	GtkWidget * InputAlign = NULL;
	GtkWidget * table = NULL;

	const gchar* sTitle;
	gint sLargeur;
	gint sHauteur;
	gint sPosition_y;
	gint sPosition_x;
	int icone;
	const gchar *fileEx = "file.wav";
	//GtkEntryBuffer * outputFileName;
	GtkWidget * play;
	GtkWidget * pause;

	/* allocation */
	lb = g_malloc( sizeof(Label) );
	Wd = g_malloc( sizeof(Widget) );

	// Title
	gtk_window_set_title(GTK_WINDOW(MainWindow), "Sound Mixer");
	// Size
	gtk_window_set_default_size(GTK_WINDOW(MainWindow), 580, 460);
	gtk_window_set_resizable (GTK_WINDOW(MainWindow), FALSE);
	// Position
	gtk_window_set_position(GTK_WINDOW (MainWindow), GTK_WIN_POS_CENTER);
	// Icone
	icone = gtk_window_set_icon_from_file(GTK_WINDOW(MainWindow), "../src/icone.png", NULL);
	// Input label
	lb->InputLabel = gtk_label_new("Input file");
	// Button file
	Wd->button_file = gtk_file_chooser_button_new (("Select a song"),GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (Wd->button_file),"../audio/");
	// Output label
	lb->OutputLabel = gtk_label_new("Output file");
	// Entry
	Wd->OutputWidget = gtk_entry_new ();
	//gtk_entry_get_buffer (GTK_ENTRY(Wd->OutputWidget));
	gtk_entry_set_max_length (GTK_ENTRY (Wd->OutputWidget), 50);
	gtk_entry_set_visibility (GTK_ENTRY(Wd->OutputWidget), TRUE);
	g_signal_connect (Wd->OutputWidget, "activate", G_CALLBACK (enter_callback),Wd->OutputWidget);
	
	play = gtk_image_new_from_file ("../src/rsz_play.jpg");
	pause = gtk_image_new_from_file ("../src/rsz_pause.jpg");
	//gtk_image_set_pixel_size (GTK_IMAGE(play), 10);
    //gtk_image_set_pixel_size (GTK_IMAGE(pause), 10);       
	
	// Play Button and pause buttons
	Wd->PlayButton = gtk_button_new();
	Wd->PauseButton = gtk_button_new();
	//Wd->PlayButton = gtk_button_new_from_icon_name("play.jpeg", GTK_ICON_SIZE_BUTTON);
	//Wd->PauseButton = gtk_button_new_from_icon_name("pause.jpeg", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_hexpand (Wd->PlayButton, TRUE);
    gtk_widget_set_halign (Wd->PlayButton, GTK_ALIGN_START);
	gtk_widget_set_hexpand (Wd->PauseButton, TRUE);
    gtk_widget_set_halign (Wd->PauseButton, GTK_ALIGN_START);
    
	gtk_button_set_image (GTK_BUTTON(Wd->PlayButton), play);
	gtk_button_set_image (GTK_BUTTON(Wd->PauseButton), pause);
	
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
	gtk_range_set_inverted(GTK_RANGE(Wd->WetWidget), TRUE);
	gtk_range_set_inverted(GTK_RANGE(Wd->ReflectWidget), TRUE);
	gtk_range_set_inverted(GTK_RANGE(Wd->DampWidget), TRUE);
	gtk_range_set_inverted(GTK_RANGE(Wd->Rt60Widget), TRUE);
	gtk_range_set_inverted(GTK_RANGE(Wd->AreaWidget), TRUE);
	gtk_range_set_inverted(GTK_RANGE(Wd->VolumeWidget), TRUE);

	// Set default values range
	gtk_range_set_value (GTK_RANGE(Wd->WetWidget), rs->wetmix);
	gtk_range_set_value (GTK_RANGE(Wd->ReflectWidget), rs->reflect);
	gtk_range_set_value (GTK_RANGE(Wd->DampWidget), rs->damping);
	gtk_range_set_value (GTK_RANGE(Wd->Rt60Widget), rs->rt60);
	gtk_range_set_value (GTK_RANGE(Wd->AreaWidget), rs->area);
	gtk_range_set_value (GTK_RANGE(Wd->VolumeWidget), rs->volume);
	
	// set Entry 
	//gtk_entry_set_buffer (GTK_ENTRY(Wd->OutputWidget), outputFileName);
	gtk_entry_set_text (GTK_ENTRY(Wd->OutputWidget), fileEx);
	
	g_signal_connect (G_OBJECT(Wd->PlayButton), "clicked", G_CALLBACK(play_button), NULL);
	g_signal_connect (G_OBJECT(Wd->PauseButton), "clicked", G_CALLBACK(pause_button), NULL);
	
	g_signal_connect(G_OBJECT(MainWindow), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT(Wd->WetWidget), "value-changed", G_CALLBACK (get_wet), &rs->wetmix);
	g_signal_connect(G_OBJECT(Wd->ReflectWidget), "value-changed", G_CALLBACK (get_reflection), NULL);
	g_signal_connect(G_OBJECT(Wd->DampWidget), "value-changed", G_CALLBACK (get_damping), NULL);
	
	g_signal_connect(G_OBJECT(Wd->Rt60Widget), "value-changed", G_CALLBACK (get_rt60), NULL);
	
	g_signal_connect(G_OBJECT(Wd->AreaWidget), "value-changed", G_CALLBACK (get_area), &rs->area);
	g_signal_connect(G_OBJECT(Wd->VolumeWidget), "value-changed", G_CALLBACK (get_volume), &rs->volume);

	g_signal_connect(G_OBJECT(Wd->button_file), "file-set", G_CALLBACK(file_selected_callback), NULL);
	gtk_container_set_border_width (GTK_CONTAINER(MainWindow), 20);

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
	gtk_grid_attach(GTK_GRID(table), lb->InputLabel, 0, 0, 3, 1);
	gtk_grid_attach(GTK_GRID(table), lb->OutputLabel, 3, 0, 3, 1);
	gtk_grid_attach(GTK_GRID(table), lb->PlayLabel, 1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->StopLabel, 3, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->WetLabel, 0, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->ReflectLabel, 1, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->DampLabel, 2, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->Rt60Label, 3, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->RoomSizeLabel, 4, 5, 2, 1);
	gtk_grid_attach(GTK_GRID(table), lb->AreaLabel, 4, 6, 1, 1);
	gtk_grid_attach(GTK_GRID(table), lb->VolumeLabel, 5, 6, 1, 1);


	gtk_grid_attach(GTK_GRID(table), Wd->WetWidget, 0, 6, 1, 5);
	gtk_grid_attach(GTK_GRID(table), Wd->ReflectWidget, 1, 6, 1, 5);
	gtk_grid_attach(GTK_GRID(table), Wd->DampWidget, 2, 6, 1, 5);
	gtk_grid_attach(GTK_GRID(table), Wd->Rt60Widget, 3, 6, 1, 5);
	gtk_grid_attach(GTK_GRID(table), Wd->AreaWidget, 4, 7, 1, 4);
	gtk_grid_attach(GTK_GRID(table), Wd->VolumeWidget, 5, 7, 1, 4);

	gtk_grid_attach(GTK_GRID(table), Wd->button_file, 0, 1, 3, 1);
	gtk_grid_attach(GTK_GRID(table), Wd->OutputWidget, 3, 1, 3, 1);

	gtk_grid_attach(GTK_GRID(table), Wd->PlayButton, 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(table), Wd->PauseButton, 3, 3, 1, 1);

	// rows
	gtk_grid_set_row_homogeneous (GTK_GRID(table), FALSE);
	gtk_grid_set_row_spacing (GTK_GRID(table), 30);
	// columms
	gtk_grid_set_column_homogeneous (GTK_GRID(table), FALSE);
	gtk_grid_set_column_spacing (GTK_GRID(table), 80);

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
	
	
	
	// Entry out
	gtk_entry_get_text (GTK_ENTRY(Wd->OutputWidget));
	gtk_editable_select_region (GTK_EDITABLE (Wd->OutputWidget), 0, 3);
	
	//image size
	//gtk_image_get_pixel_size (GTK_IMAGE(play));
	//gtk_image_get_pixel_size (GTK_IMAGE(pause));
	
	// Display images of the buttons
	gtk_button_get_image (GTK_BUTTON(Wd->PlayButton));
	gtk_button_get_image (GTK_BUTTON(Wd->PauseButton));

	
	/* Display and event loops */
	printf("\n-------\nWindow\n------- \n \nTitle: %s \nSize: %d x %d \nPosition: %dy %dx\n ", sTitle, sLargeur, sHauteur, sPosition_y, sPosition_x);

	//return EXIT_SUCCESS;
}
