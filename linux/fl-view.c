/*
 * Copyright (C) 2020 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 or version 3 of the License.
 * See http://www.gnu.org/copyleft/lgpl.html the full text of the license.
 */

#include "embedder.h"
#include "fl-view.h"

typedef struct
{
    gchar *assets_path;
    gchar *icu_data_path;
    FlutterEngine engine;
} FlViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (FlView, fl_view, GTK_TYPE_GL_AREA)

static gchar *
flutter_engine_result_to_string (FlutterEngineResult result)
{
    switch (result)
    {
    case kSuccess:
        return g_strdup ("Success");
    case kInvalidLibraryVersion:
        return g_strdup ("Invalid library version");
    case kInvalidArguments:
        return g_strdup ("Invalid arguments");
    case kInternalInconsistency:
        return g_strdup ("Internal inconsistency");
    default:
        return g_strdup_printf ("Unknown Flutter error %d", result);
    }
}

static bool
fl_view_gl_make_current (void *user_data)
{
    FlView *self = user_data;
    gtk_gl_area_make_current (GTK_GL_AREA (self));
    return true;
}

static bool
fl_view_gl_clear_current (void *user_data)
{
    //FlView *self = user_data;
    g_printerr ("fl_view_gl_clear_current\n");
    return false;
}

static bool
fl_view_gl_present (void *user_data)
{
    //FlView *self = user_data;
    g_printerr ("fl_view_gl_present\n");
    return false;
}

static uint32_t
fl_view_gl_fbo_callback (void *user_data)
{
    //FlView *self = user_data;
    g_printerr ("fl_view_gl_fbo_callback\n");
    return 0;
}

static bool
fl_view_gl_make_resource_current (void *user_data)
{
    //FlView *self = user_data;
    g_printerr ("fl_view_gl_make_resource_current\n");
    return false;
}

static void
fl_view_dispose (GObject *object)
{
    FlViewPrivate *priv = fl_view_get_instance_private (FL_VIEW (object));

    g_clear_pointer (&priv->assets_path, g_free);
    g_clear_pointer (&priv->icu_data_path, g_free);

    G_OBJECT_CLASS (fl_view_parent_class)->dispose (object);
}

static void
fl_view_realize (GtkWidget *widget)
{
    FlView *self = FL_VIEW (widget);
    FlViewPrivate *priv = fl_view_get_instance_private (self);
    FlutterRendererConfig config = { 0 };
    FlutterProjectArgs args = { 0 };

    GTK_WIDGET_CLASS (fl_view_parent_class)->realize (widget);

    config.type = kOpenGL;
    config.open_gl.struct_size = sizeof (FlutterOpenGLRendererConfig);
    config.open_gl.make_current = fl_view_gl_make_current;
    config.open_gl.clear_current = fl_view_gl_clear_current;
    config.open_gl.present = fl_view_gl_present;
    config.open_gl.fbo_callback = fl_view_gl_fbo_callback;
    config.open_gl.make_resource_current = fl_view_gl_make_resource_current;
    args.struct_size = sizeof (FlutterProjectArgs);
    args.assets_path = priv->assets_path;
    args.icu_data_path = priv->icu_data_path;

    FlutterEngineResult result = FlutterEngineInitialize (FLUTTER_ENGINE_VERSION, &config, &args, self, &priv->engine);
    if (result != kSuccess) {
        g_autofree gchar *error = flutter_engine_result_to_string (result);
        g_warning ("Failed to initialize Flutter: %s", error);
        return;
    }

    result = FlutterEngineRunInitialized (priv->engine);
    if (result != kSuccess) {
        g_autofree gchar *error = flutter_engine_result_to_string (result);
        g_warning ("Failed to run Flutter: %s", error);
        return;
    }
}

static gboolean
fl_view_render (GtkGLArea *widget, GdkGLContext *context)
{
    g_printerr ("fl_view_render\n");
    return FALSE;
}

static void
fl_view_class_init (FlViewClass *klass)
{
    G_OBJECT_CLASS (klass)->dispose = fl_view_dispose;
    GTK_WIDGET_CLASS (klass)->realize = fl_view_realize;
    GTK_GL_AREA_CLASS (klass)->render = fl_view_render;
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

void
fl_view_set_assets_path (FlView *self, const gchar *assets_path)
{
    FlViewPrivate *priv = fl_view_get_instance_private (self);

    g_return_if_fail (FL_IS_VIEW (self));

    g_free (priv->assets_path);
    priv->assets_path = g_strdup (assets_path);
}

void
fl_view_set_icu_data_path (FlView *self, const gchar *icu_data_path)
{
    FlViewPrivate *priv = fl_view_get_instance_private (self);

    g_return_if_fail (FL_IS_VIEW (self));

    g_free (priv->icu_data_path);
    priv->icu_data_path = g_strdup (icu_data_path);
}
