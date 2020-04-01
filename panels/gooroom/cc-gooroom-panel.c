/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 *
 * Copyright (C) 2020 gooroom <gooroom@gooroom.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

#include "cc-gooroom-panel.h"
#include "cc-gooroom-resources.h"

#define GOOROOM_SCHEMA "org.gnome.desktop.gooroom"

struct _CcGooroomPanel
{
  CcPanel parent_instance;

  GSettings *gooroom_settings;
  GtkWidget *gooroom_scrolled_window;
  GtkWidget *gooroom_web_view;
};

G_DEFINE_TYPE (CcGooroomPanel, cc_gooroom_panel, CC_TYPE_PANEL)

static const char *
cc_gooroom_panel_get_help_uri (CcPanel *self)
{
  return "help:gnome-help/gooroom";
}

static void
cc_gooroom_panel_dispose (GObject *object)
{
  CcGooroomPanel *gooroom_panel = CC_GOOROOM_PANEL (object);

  g_clear_object (&gooroom_panel->gooroom_settings);

  G_OBJECT_CLASS (cc_gooroom_panel_parent_class)->dispose (object);
}

static void
cc_gooroom_panel_class_init (CcGooroomPanelClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/gooroom/gooroom.ui");
  gtk_widget_class_bind_template_child (widget_class, CcGooroomPanel, gooroom_scrolled_window);

  G_OBJECT_CLASS (klass)->dispose = cc_gooroom_panel_dispose;
}

static void
gooroom_panel_setup (CcGooroomPanel *self)
{
  self->gooroom_settings = g_settings_new (GOOROOM_SCHEMA);
  self->gooroom_web_view = webkit_web_view_new ();
  webkit_web_view_load_uri (self->gooroom_web_view, g_settings_get_string (self->gooroom_settings, "gooroom-url"));
  gtk_container_add (GTK_CONTAINER (self->gooroom_scrolled_window), self->gooroom_web_view);
  gtk_widget_show (self->gooroom_web_view);
}

static void
cc_gooroom_panel_init (CcGooroomPanel *self)
{
  GtkWidget *w;

  g_resources_register (cc_gooroom_get_resource ());

  gtk_widget_init_template (GTK_WIDGET (self));

  gooroom_panel_setup (self);
}

GtkWidget *
cc_gooroom_panel_new (void)
{
  return g_object_new (CC_TYPE_GOOROOM_PANEL,
                       NULL);
}
