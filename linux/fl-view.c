/*
 * Copyright (C) 2020 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 or version 3 of the License.
 * See http://www.gnu.org/copyleft/lgpl.html the full text of the license.
 */

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <gdk/gdkx.h>

#include "embedder.h"
#include "fl-view.h"

typedef struct
{
    EGLDisplay *egl_display;
    EGLSurface *egl_surface;
    EGLContext *egl_context;

    gchar *assets_path;
    gchar *icu_data_path;

    FlutterEngine engine;
    GLuint fbo;
} FlViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (FlView, fl_view, GTK_TYPE_DRAWING_AREA)

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
    FlViewPrivate *priv = fl_view_get_instance_private (self);
    g_printerr ("fl_view_gl_make_current\n");
    if (!eglMakeCurrent (priv->egl_display, priv->egl_surface, priv->egl_surface, priv->egl_context))
       g_critical ("Failed to make EGL context current");
    return true;
}

static bool
fl_view_gl_clear_current (void *user_data)
{
    FlView *self = user_data;
    FlViewPrivate *priv = fl_view_get_instance_private (self);
    g_printerr ("fl_view_gl_clear_current\n");
    eglMakeCurrent (priv->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
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
    FlView *self = user_data;
    FlViewPrivate *priv = fl_view_get_instance_private (self);
    g_printerr ("fl_view_gl_fbo_callback\n");
    return priv->fbo;
}

/*static bool
fl_view_gl_make_resource_current (void *user_data)
{
    //FlView *self = user_data;
    g_printerr ("fl_view_gl_make_resource_current\n");
    return false;
}*/

static void *
fl_view_gl_proc_resolver (void *user_data, const char *name)
{
    return eglGetProcAddress (name);
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
    EGLint egl_major, egl_minor;
    EGLConfig egl_config;
    EGLint n_config;
    EGLint attributes[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                            EGL_RED_SIZE, 8,
                            EGL_GREEN_SIZE, 8,
                            EGL_BLUE_SIZE, 8,
                            EGL_ALPHA_SIZE, 8,
                            EGL_NONE };
    FlutterRendererConfig config = { 0 };
    FlutterProjectArgs args = { 0 };

    GTK_WIDGET_CLASS (fl_view_parent_class)->realize (widget);

    priv->egl_display = eglGetDisplay ((EGLNativeDisplayType) gdk_x11_display_get_xdisplay (gtk_widget_get_display (widget)));
    if (!eglInitialize (priv->egl_display, &egl_major, &egl_minor))
        g_critical ("Failed to initialze EGL");
    g_printerr ("Initialized EGL version %d.%d\n", egl_major, egl_minor);
    if (!eglChooseConfig (priv->egl_display, attributes, &egl_config, 1, &n_config))
        g_critical ("Failed to choose EGL config");
    if (n_config == 0)
        g_critical ("Failed to find appropriate EGL config");
    if (!eglBindAPI (EGL_OPENGL_ES_API))
        g_critical ("Failed to bind EGL OpenGL ES API");
    priv->egl_surface = eglCreateWindowSurface (priv->egl_display, egl_config, gdk_x11_window_get_xid (gtk_widget_get_window (widget)), NULL);
    EGLint context_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2,
                                    EGL_NONE };
    priv->egl_context = eglCreateContext (priv->egl_display, egl_config, EGL_NO_CONTEXT, context_attributes);
    EGLint value;
    eglQueryContext (priv->egl_display, priv->egl_context, EGL_CONTEXT_CLIENT_VERSION, &value);

    config.type = kOpenGL;
    config.open_gl.struct_size = sizeof (FlutterOpenGLRendererConfig);
    config.open_gl.make_current = fl_view_gl_make_current;
    config.open_gl.clear_current = fl_view_gl_clear_current;
    config.open_gl.present = fl_view_gl_present;
    config.open_gl.fbo_callback = fl_view_gl_fbo_callback;
    config.open_gl.make_resource_current = NULL;//fl_view_gl_make_resource_current;
    config.open_gl.gl_proc_resolver = fl_view_gl_proc_resolver;
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

static void
fl_view_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
    FlView *self = FL_VIEW (widget);
    FlViewPrivate *priv = fl_view_get_instance_private (self);

    g_printerr ("fl_view_size_allocate %d %d\n", allocation->width, allocation->height);

    GLuint tx;
    glGenTextures(1, &tx);
    //glGenTextures(1, &front_buffer_tx_);
    GLuint rb;
    glGenRenderbuffers (1, &rb);
    glBindRenderbuffer (GL_RENDERBUFFER, rb);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, allocation->width, allocation->height);
    glGenFramebuffers (1, &priv->fbo);
    glBindFramebuffer (GL_FRAMEBUFFER, priv->fbo);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_BINDING_2D, tx, 0);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    FlutterWindowMetricsEvent event = {};
    event.struct_size = sizeof (FlutterWindowMetricsEvent);
    event.width = allocation->width;
    event.height = allocation->height;
    event.pixel_ratio = 1; // FIXME
    FlutterEngineSendWindowMetricsEvent (priv->engine, &event);
}

static void
fl_view_class_init (FlViewClass *klass)
{
    G_OBJECT_CLASS (klass)->dispose = fl_view_dispose;
    GTK_WIDGET_CLASS (klass)->realize = fl_view_realize;
    GTK_WIDGET_CLASS (klass)->size_allocate = fl_view_size_allocate;
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
