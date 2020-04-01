#include <config.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "cc-gooroom-panel.h"
#include "cc-gooroom-resources.h"

#define GOOROOM_SCHEMA "org.gnome.desktop.gooroom"

struct _CcGooroomPanel
{
  CcPanel parent_instance;

  GtkBuilder *builder;

  GSettings *gooroom_settings;
  GtkWidget *gooroom_scrolled_window;
  GtkWidget *gooroom_web_view;
};

CC_PANEL_REGISTER (CcGooroomPanel, cc_gooroom_panel)

static const char *
cc_gooroom_panel_get_help_uri (CcPanel *panel)
{
  return "help:gnome-help/gooroom";
}

static void
cc_gooroom_panel_dispose (GObject *object)
{
  CcGooroomPanel *panel = CC_GOOROOM_PANEL (object);

  g_clear_object (&panel->builder);
  
  g_clear_object (&panel->gooroom_settings);

  G_OBJECT_CLASS (cc_gooroom_panel_parent_class)->dispose (object);
}

static void
cc_gooroom_panel_finalize (GObject *object)
{
  CcGooroomPanel *panel = CC_GOOROOM_PANEL (object);

  G_OBJECT_CLASS (cc_gooroom_panel_parent_class)->finalize (object);
}

static void
cc_gooroom_panel_class_init (CcGooroomPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  CcPanelClass *panel_class = CC_PANEL_CLASS (klass);

  object_class->dispose = cc_gooroom_panel_dispose;
}

static void
cc_gooroom_panel_init (CcGooroomPanel *panel)
{
  GtkWidget *w;

  g_resources_register (cc_gooroom_get_resource ());

  panel->builder = gtk_builder_new_from_resource ("/org/gnome/control-center/gooroom/gooroom.ui");
  if (!panel->builder)
  {
    g_warning ("Could not load ui\n");
    return;
  }

  panel->gooroom_web_view = webkit_web_view_new ();

  panel->gooroom_settings = g_settings_new (GOOROOM_SCHEMA);
  panel->gooroom_scrolled_window = (GtkWidget *) gtk_builder_get_object (panel->builder, "gooroom_scrolled_window");

  webkit_web_view_load_uri (panel->gooroom_web_view, g_settings_get_string (panel->gooroom_settings, "gooroom-url"));
  //webkit_web_view_load_uri (panel->gooroom_web_view, "http://www.hancom.com");
  gtk_container_add (GTK_CONTAINER (panel->gooroom_scrolled_window), panel->gooroom_web_view);
  gtk_container_add (GTK_CONTAINER (panel), panel->gooroom_scrolled_window);
  gtk_widget_show_all (GTK_WIDGET (panel));
}
