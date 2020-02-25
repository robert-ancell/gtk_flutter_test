/*
 * Copyright (C) 2020 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 or version 3 of the License.
 * See http://www.gnu.org/copyleft/lgpl.html the full text of the license.
 */

#include "fl-view.h"

typedef struct
{
    int placeholder;
} FlViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (FlView, fl_view, GTK_TYPE_GL_AREA)

static gboolean
fl_view_render (GtkGLArea *widget, GdkGLContext *context)
{
    return FALSE;
}

static void
fl_view_class_init (FlViewClass *klass)
{
    GtkGLAreaClass *gl_area_class = GTK_GL_AREA_CLASS (klass);

    gl_area_class->render = fl_view_render;
}

static void
fl_view_init (FlView *self)
{
}

FlView *
fl_view_new (void)
{
    return g_object_new (fl_view_get_type (), NULL);
}
