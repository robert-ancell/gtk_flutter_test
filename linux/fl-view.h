#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

G_DECLARE_DERIVABLE_TYPE (FlView, gs_view, FL, VIEW, GtkGLArea)

struct _FlViewClass
{
    GtkGLAreaClass parent_class;
};

FlView *fl_view_new (void);

G_END_DECLS
