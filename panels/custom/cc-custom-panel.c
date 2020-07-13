/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 *
 * Copyright (C) 2020 gooroom <gooroom@gooroom.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by * the Free Software Foundation; either version 2 of the License, or * (at your option) any later version.
 * * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

#include <webkit2/webkit2.h>
#include "cc-custom-panel.h"
#include "cc-custom-resources.h"

#define CUSTOM_SCHEMA "org.gnome.desktop.custom"

struct _CcCustomPanel
{
  CcPanel    parent_instance;

  GSettings *custom_settings;
  GtkWidget *custom_scrolled_window;
  GtkWidget *custom_web_view;
};

G_DEFINE_TYPE (CcCustomPanel, cc_custom_panel, CC_TYPE_PANEL)

static const char *
cc_custom_panel_get_help_uri (CcPanel *self)
{
  return "help:gnome-help/custom";
}

static void
cc_custom_panel_dispose (GObject *object)
{
  CcCustomPanel *custom_panel = CC_CUSTOM_PANEL (object);

  g_clear_object (&custom_panel->custom_settings);

  G_OBJECT_CLASS (cc_custom_panel_parent_class)->dispose (object);
}

static void
lsf_set_settings_handler (WebKitUserContentManager *manager,
                          WebKitJavascriptResult   *js_result,
                          gpointer                  user_data)
{
  JSCValue *val = webkit_javascript_result_get_js_value (js_result);
  g_print ("** lsf_set_settings handler. **\n");
  g_print ("%s\n", jsc_value_to_string (val));
}

static void
lsf_get_settings_handler (WebKitUserContentManager *manager,
                          WebKitJavascriptResult   *js_result,
                          gpointer                  user_data)
{
  JSCValue *val = webkit_javascript_result_get_js_value (js_result);
  g_print ("** lsf_get_settings handler. **\n");
  g_print ("%s\n", jsc_value_to_string (val));
}

static void
cc_custom_panel_constructed (GObject *object)
{
  CcCustomPanel *self = CC_CUSTOM_PANEL (object);
  int i = 0;
  int c;
  FILE *fp;

  self->custom_settings = g_settings_new (CUSTOM_SCHEMA);
  WebKitUserContentManager *manager = webkit_user_content_manager_new ();
  g_signal_connect (manager, "script-message-received::lsf_set_settings", G_CALLBACK (lsf_set_settings_handler), NULL);
  g_signal_connect (manager, "script-message-received::lsf_get_settings", G_CALLBACK (lsf_get_settings_handler), NULL);
  webkit_user_content_manager_register_script_message_handler (manager, "lsf_set_settings");
  webkit_user_content_manager_register_script_message_handler (manager, "lsf_get_settings");
  self->custom_web_view = webkit_web_view_new_with_user_content_manager (manager);
  webkit_web_view_load_uri (WEBKIT_WEB_VIEW (self->custom_web_view),
                            g_settings_get_string (self->custom_settings, "custom-html"));
  gtk_container_add (GTK_CONTAINER (self->custom_scrolled_window), self->custom_web_view);
  gtk_widget_show (self->custom_web_view);
}

static void
cc_custom_panel_class_init (CcCustomPanelClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = cc_custom_panel_dispose;
  object_class->constructed = cc_custom_panel_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/custom/custom.ui");
  gtk_widget_class_bind_template_child (widget_class, CcCustomPanel, custom_scrolled_window);
}

static void
cc_custom_panel_init (CcCustomPanel *self)
{
  g_resources_register (cc_custom_get_resource ());

  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
cc_custom_panel_new (void)
{
  return g_object_new (CC_TYPE_CUSTOM_PANEL,
                       NULL);
}
