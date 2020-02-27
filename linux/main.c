#include <gtk/gtk.h>

#include "fl-view.h"

int
main (int argc, char **argv)
{
    gtk_init (&argc, &argv);

    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_show (window);

    FlView *view = fl_view_new ();
    fl_view_set_assets_path (view, "./build/flutter_assets");
    fl_view_set_icu_data_path (view, "./linux/flutter/ephemeral/icudtl.dat");

    gtk_widget_show (GTK_WIDGET (view));
    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (view));

    gtk_main ();

    return 0;
}
