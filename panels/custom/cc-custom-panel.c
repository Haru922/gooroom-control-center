/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
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

#include <sys/stat.h>
#include <webkit2/webkit2.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/json_util.h>
#include "cc-custom-panel.h"
#include "cc-custom-resources.h"

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
lsf_msg_handler (WebKitUserContentManager *manager,
                 WebKitJavascriptResult   *js_result,
                 gpointer                  user_data)
{
  struct json_object *req_obj;
  struct json_object *resp_obj;
  struct json_object *prop_obj;
  lsf_user_data_t app_data;
  const char *param;
  const char *func;
  CcCustomPanel *self = (CcCustomPanel *) user_data;
  JSCValue *val = webkit_javascript_result_get_js_value (js_result);
  char *req_msg = (char *) calloc (1, JSON_FILE_BUF_SIZE);
  char *response = NULL;
  char script[JSON_FILE_BUF_SIZE] = { 0, };
  int ret;

  fprintf (stdout, "** from web view: %s **\n", jsc_value_to_string (val)); // TODO DELETE: Debugging Message.

  req_obj = json_tokener_parse (jsc_value_to_string (val));
  if (!req_obj)
    return;

  json_object_object_get_ex (req_obj, "method", &prop_obj);
  func = json_object_get_string (prop_obj);

  if (strcmp (func, "set_settings") == 0)
  {
    json_object_object_get_ex (req_obj, "app_conf", &prop_obj);
    param = json_object_get_string (prop_obj);
    snprintf (req_msg, JSON_FILE_BUF_SIZE, SET_SETTINGS_FMT,
              APP_DBUS_NAME, CC_DBUS_NAME,
              lsf_panel_access_token, func, param);
    ret = lsf_send_message (lsf_panel_symm_key, req_msg, &response);
  }
  else if (strcmp (func, "get_settings") == 0)
  {
    snprintf (req_msg, JSON_FILE_BUF_SIZE, GET_SETTINGS_FMT,
              APP_DBUS_NAME, CC_DBUS_NAME,
              lsf_panel_access_token, func);
    ret = lsf_send_message (lsf_panel_symm_key, req_msg, &response);
    if (ret == LSF_MESSAGE_RE_AUTH)
    {
      ret = lsf_auth (&app_data, PASS_PHRASE);
      if (ret == LSF_AUTH_STAT_OK)
      {
        lsf_panel_symm_key = g_strdup (app_data.symm_key);
        lsf_panel_access_token = g_strdup (app_data.access_token);
      }
    }
    else if (ret == LSF_MESSAGE_RESP_OK)
    {
      resp_obj = json_tokener_parse (response);
      fprintf (stdout, "-- from testapp: %s --\n", response); // TODO DELETE: Debugging Message.
      snprintf(script, JSON_FILE_BUF_SIZE,
               "localStorage.setItem('lsf_msg', '%s')",
               json_object_get_string (resp_obj));
      webkit_web_view_run_javascript (WEBKIT_WEB_VIEW (self->custom_web_view),
                                      script, NULL, NULL, NULL);
      json_object_put (resp_obj);
    }
  }
  fprintf (stdout, "++ request message: %s ++\n", req_msg); // TODO DELETE: Debugging Message.

  FREE (response);
  FREE (req_msg);
  json_object_put (req_obj);
}

static void
cc_custom_panel_constructed (GObject *object)
{
  CcCustomPanel *self = CC_CUSTOM_PANEL (object);
  WebKitUserContentManager *manager = webkit_user_content_manager_new ();
  WebKitUserScript *script;
  struct stat st;
  char *lsf_api = NULL;
  FILE *fp = fopen ("/var/tmp/lsf/js/lsf.js", "r");

  if (g_resource_load ("/org/gnome/control-center/custom/resources/js/lsf.js", NULL))
    g_print ("loaded\n");

  if (fp) 
  {
    fstat (fileno (fp), &st);
    lsf_api = (char *) calloc (1, st.st_size + 1);
    fread (lsf_api, st.st_size, 1, fp);
    lsf_api[st.st_size] = '\0';
    fclose (fp);
  }

  self->custom_settings = g_settings_new (CUSTOM_SCHEMA);
  g_signal_connect (manager, "script-message-received::lsf_interface",
                    G_CALLBACK (lsf_msg_handler), self);
  webkit_user_content_manager_register_script_message_handler (manager, "lsf_interface");

  if (lsf_api) 
  {
    script = webkit_user_script_new (lsf_api, 0, 0, NULL, NULL);
    webkit_user_content_manager_add_script (manager, script);
    FREE (lsf_api);
  }
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

  gtk_widget_class_set_template_from_resource (widget_class, CUSTOM_UI);
  gtk_widget_class_bind_template_child (widget_class, CcCustomPanel, custom_scrolled_window);
}

static void
cc_custom_panel_init (CcCustomPanel *self)
{
  lsf_user_data_t app_data;
  int ret;
  g_resources_register (cc_custom_get_resource ());

  gtk_widget_init_template (GTK_WIDGET (self));

  ret = lsf_auth (&app_data, PASS_PHRASE);
  if (ret == LSF_AUTH_STAT_OK)
  {
    lsf_panel_symm_key = g_strdup (app_data.symm_key);
    lsf_panel_access_token = g_strdup (app_data.access_token);
  }
}

GtkWidget *
cc_custom_panel_new (void)
{
  return g_object_new (CC_TYPE_CUSTOM_PANEL,
                       NULL);
}
