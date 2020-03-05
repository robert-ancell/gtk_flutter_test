/*
 * Copyright (C) 2020 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 or version 3 of the License.
 * See http://www.gnu.org/copyleft/lgpl.html the full text of the license.
 */

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

G_DECLARE_DERIVABLE_TYPE (FlView, fl_view, FL, VIEW, GtkDrawingArea)

struct _FlViewClass
{
    GtkDrawingAreaClass parent_class;
};

FlView *fl_view_new              (void);

void    fl_view_set_assets_path   (FlView *view, const gchar *assets_path);

void    fl_view_set_icu_data_path (FlView *view, const gchar *icu_data_path);

G_END_DECLS
