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
#include "cc-security-apps-panel.h"
#include "cc-security-apps-resources.h"

struct _CcSecurityAppsPanel
{
  CcPanel    parent_instance;

  GtkWidget *security_apps_notebook;
  GtkWidget *web_view[SECURITY_APPS_MAX];
  gchar *app_dbus_name[SECURITY_APPS_MAX];
};

G_DEFINE_TYPE (CcSecurityAppsPanel, cc_security_apps_panel, CC_TYPE_PANEL)

static void
cc_security_apps_panel_dispose (GObject *object)
{
  CcSecurityAppsPanel *security_apps_panel = CC_SECURITY_APPS_PANEL (object);

  G_OBJECT_CLASS (cc_security_apps_panel_parent_class)->dispose (object);
}

static char *
get_app_name (char *dbus_name)
{
  int i, j;
  char *app_name = (char *) malloc (sizeof (char) * 255);
  for (i = 0, j = 0; dbus_name[i]; i++) {
    if (dbus_name[i] == '.')
      j = 0;
    else
      app_name[j++] = dbus_name[i];
  }
  app_name[j] = '\0';
  return app_name;
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
  const char *app_settings;
  const char *method;
  JSCValue *val = webkit_javascript_result_get_js_value (js_result);
  CcSecurityAppsPanel *self = CC_SECURITY_APPS_PANEL (user_data);
  char *req_msg = NULL;
  char *response = NULL;
  char script[JSON_FILE_BUF_SIZE] = { 0, };
  int ret;
  int app_num;

  app_num = gtk_notebook_get_current_page (GTK_NOTEBOOK (self->security_apps_notebook));

  req_obj = json_tokener_parse (jsc_value_to_string (val));
  if (!req_obj)
    return;

  json_object_object_get_ex (req_obj, "method", &prop_obj);
  method = json_object_get_string (prop_obj);

  req_msg = (char *) calloc (1, JSON_FILE_BUF_SIZE);
  if (!strcmp (method, "lsf_set_settings"))
  {
    json_object_object_get_ex (req_obj, "app_conf", &prop_obj);
    app_settings = json_object_get_string (prop_obj);
    snprintf (req_msg, JSON_FILE_BUF_SIZE, SET_SETTINGS_FMT,
              self->app_dbus_name[app_num], CC_DBUS_NAME,
              lsf_panel_access_token, method, app_settings);
    ret = lsf_send_message (lsf_panel_symm_key, req_msg, &response);
  }
  else if (!strcmp (method, "lsf_get_settings"))
  {
    snprintf (req_msg, JSON_FILE_BUF_SIZE, GET_SETTINGS_FMT,
              self->app_dbus_name[app_num], CC_DBUS_NAME,
              lsf_panel_access_token, method);
    ret = lsf_send_message (lsf_panel_symm_key, req_msg, &response);
  }
  fprintf (stdout, "\n-- request message: %s\n", req_msg);
  json_object_put (req_obj);
  FREE (req_msg);

  if (ret == LSF_MESSAGE_RE_AUTH)
  {
    ret = lsf_auth (&app_data, CC_PASSPHRASE);
    if (ret == LSF_AUTH_STAT_OK)
    {
      lsf_panel_symm_key = g_strdup (app_data.symm_key);
      lsf_panel_access_token = g_strdup (app_data.access_token);
    }
  }
  else if (ret == LSF_MESSAGE_RESP_OK)
  {
    fprintf (stdout, "\n++ return message: %s\n", response);
    resp_obj = json_tokener_parse (response);
    snprintf(script, JSON_FILE_BUF_SIZE,
             "localStorage.setItem('lsfMsg', '%s')",
             json_object_get_string (resp_obj));
    webkit_web_view_run_javascript (WEBKIT_WEB_VIEW (WEBKIT_WEB_VIEW (self->web_view[app_num])),
                                    script, NULL, NULL, NULL);
    json_object_put (resp_obj);
  }

  FREE (response);
}

static void
cc_security_apps_panel_constructed (GObject *object)
{
  CcSecurityAppsPanel *self = CC_SECURITY_APPS_PANEL (object);
  WebKitUserContentManager *manager = webkit_user_content_manager_new ();
  WebKitUserScript *lsf_api;
  GtkWidget *scrolled_window;
  const char *script;
  GDir *dir = NULL;
  const gchar *app_dir;
  gchar *app_name;
  gchar *app_html;
  int app_num = 0;

  script = "function lsfGetSettings() {\
              let obj = { method: \"lsf_get_settings\" };\
              window.webkit.messageHandlers.lsfInterface.postMessage(JSON.stringify(obj));\
              return JSON.parse(localStorage.getItem('lsfMsg')); }\
            function lsfSetSettings(arg) {\
              let obj = { method: \"lsf_set_settings\",\
                          app_conf: arg };\
              window.webkit.messageHandlers.lsfInterface.postMessage(JSON.stringify(obj));\
              localStorage.removeItem('lsfMsg');\
              return lsfGetSettings(); }\
            window.addEventListener('unload', function(e) { localStorage.clear(); });";
  lsf_api = webkit_user_script_new (script, 0, 0, NULL, NULL);

  dir = g_dir_open (LSF_CC_PANEL_DIR, 0, NULL);
  while ((app_dir = g_dir_read_name (dir)) != NULL) {
    self->app_dbus_name[app_num] = g_strdup (app_dir);
    manager = webkit_user_content_manager_new ();
    webkit_user_content_manager_add_script (manager, lsf_api);
    g_signal_connect (manager, "script-message-received::lsfInterface",
                      G_CALLBACK (lsf_msg_handler), self);
    webkit_user_content_manager_register_script_message_handler (manager, "lsfInterface");

    app_name = get_app_name (self->app_dbus_name[app_num]);
    app_html = g_strconcat ("file://", LSF_CC_PANEL_DIR, "/", self->app_dbus_name[app_num], "/html/", app_name, ".html", NULL);
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    self->web_view[app_num] = webkit_web_view_new_with_user_content_manager (manager);
    webkit_web_view_load_uri (WEBKIT_WEB_VIEW (self->web_view[app_num]), app_html);
    gtk_container_add (GTK_CONTAINER (scrolled_window), self->web_view[app_num++]);
    gtk_notebook_append_page (GTK_NOTEBOOK (self->security_apps_notebook), scrolled_window, gtk_label_new (app_name));
    g_free (app_name);
    g_free (app_html);
  }
  g_dir_close (dir);

  gtk_widget_show_all (self->security_apps_notebook);
}

static void
cc_security_apps_panel_class_init (CcSecurityAppsPanelClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = cc_security_apps_panel_dispose;
  object_class->constructed = cc_security_apps_panel_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, SECURITY_APPS_UI);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityAppsPanel, security_apps_notebook);
}

static void
cc_security_apps_panel_init (CcSecurityAppsPanel *self)
{
  lsf_user_data_t app_data;
  int ret;

  g_resources_register (cc_security_apps_get_resource ());
  gtk_widget_init_template (GTK_WIDGET (self));

  ret = lsf_auth (&app_data, CC_PASSPHRASE);
  if (ret == LSF_AUTH_STAT_OK)
  {
    lsf_panel_symm_key = g_strdup (app_data.symm_key);
    lsf_panel_access_token = g_strdup (app_data.access_token);
  }
}

GtkWidget *
cc_security_apps_panel_new (void)
{
  return g_object_new (CC_TYPE_SECURITY_APPS_PANEL,
                       NULL);
}
