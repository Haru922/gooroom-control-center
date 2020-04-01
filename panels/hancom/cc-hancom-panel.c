#include <config.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "cc-hancom-panel.h"
#include "cc-hancom-resources.h"

#define HANCOM_SCHEMA "org.gnome.desktop.hancom"

struct _CcHancomPanel
{
  CcPanel parent_instance;

  GtkBuilder *builder;

  GSettings *hancom_settings;
  GtkWidget *hancom_scrolled_window;
  GtkWidget *hancom_web_view;
};

CC_PANEL_REGISTER (CcHancomPanel, cc_hancom_panel)

static const char *
cc_hancom_panel_get_help_uri (CcPanel *panel)
{
  return "help:gnome-help/hancom";
}

static void
cc_hancom_panel_dispose (GObject *object)
{
  CcHancomPanel *panel = CC_HANCOM_PANEL (object);

  g_clear_object (&panel->builder);
  
  g_clear_object (&panel->hancom_settings);

  G_OBJECT_CLASS (cc_hancom_panel_parent_class)->dispose (object);
}

static void
cc_hancom_panel_finalize (GObject *object)
{
  CcHancomPanel *panel = CC_HANCOM_PANEL (object);

  G_OBJECT_CLASS (cc_hancom_panel_parent_class)->finalize (object);
}

static void
cc_hancom_panel_class_init (CcHancomPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  CcPanelClass *panel_class = CC_PANEL_CLASS (klass);

  object_class->dispose = cc_hancom_panel_dispose;
}

static void
cc_hancom_panel_init (CcHancomPanel *panel)
{
  GtkWidget *w;

  g_resources_register (cc_hancom_get_resource ());

  panel->builder = gtk_builder_new_from_resource ("/org/gnome/control-center/hancom/hancom.ui");
  if (!panel->builder)
  {
    g_warning ("Could not load ui\n");
    return;
  }

  panel->hancom_web_view = webkit_web_view_new ();

  panel->hancom_settings = g_settings_new (HANCOM_SCHEMA);
  panel->hancom_scrolled_window = (GtkWidget *) gtk_builder_get_object (panel->builder, "hancom_scrolled_window");

  webkit_web_view_load_uri (panel->hancom_web_view, g_settings_get_string (panel->hancom_settings, "hancom-url"));
  //webkit_web_view_load_uri (panel->hancom_web_view, "http://www.hancom.com");
  gtk_container_add (GTK_CONTAINER (panel->hancom_scrolled_window), panel->hancom_web_view);
  gtk_container_add (GTK_CONTAINER (panel), panel->hancom_scrolled_window);
  gtk_widget_show_all (GTK_WIDGET (panel));
}
