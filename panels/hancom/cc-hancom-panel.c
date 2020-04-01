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

#include "cc-hancom-panel.h"
#include "cc-hancom-resources.h"

#define HANCOM_SCHEMA "org.gnome.desktop.hancom"

struct _CcHancomPanel
{
  CcPanel parent_instance;

  GSettings *hancom_settings;
  GtkWidget *hancom_scrolled_window;
  GtkWidget *hancom_web_view;
};

G_DEFINE_TYPE (CcHancomPanel, cc_hancom_panel, CC_TYPE_PANEL)

static const char *
cc_hancom_panel_get_help_uri (CcPanel *self)
{
  return "help:gnome-help/hancom";
}

static void
cc_hancom_panel_dispose (GObject *object)
{
  CcHancomPanel *hancom_panel = CC_HANCOM_PANEL (object);

  g_clear_object (&hancom_panel->hancom_settings);

  G_OBJECT_CLASS (cc_hancom_panel_parent_class)->dispose (object);
}

static void
cc_hancom_panel_class_init (CcHancomPanelClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/hancom/hancom.ui");
  gtk_widget_class_bind_template_child (widget_class, CcHancomPanel, hancom_scrolled_window);

  G_OBJECT_CLASS (klass)->dispose = cc_hancom_panel_dispose;
}

static void
hancom_panel_setup (CcHancomPanel *self)
{
  self->hancom_settings = g_settings_new (HANCOM_SCHEMA);
  self->hancom_web_view = webkit_web_view_new ();
  webkit_web_view_load_uri (self->hancom_web_view, g_settings_get_string (self->hancom_settings, "hancom-url"));
  gtk_container_add (GTK_CONTAINER (self->hancom_scrolled_window), self->hancom_web_view);
  gtk_widget_show (self->hancom_web_view);
}

static void
cc_hancom_panel_init (CcHancomPanel *self)
{
  GtkWidget *w;

  g_resources_register (cc_hancom_get_resource ());

  gtk_widget_init_template (GTK_WIDGET (self));

  hancom_panel_setup (self);
}

GtkWidget *
cc_hancom_panel_new (void)
{
  return g_object_new (CC_TYPE_HANCOM_PANEL,
                       NULL);
}
