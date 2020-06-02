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
#include <math.h>

#include "cc-security-framework-panel.h"
#include "cc-security-framework-resources.h"

#define SECURITY_FRAMEWORK_SCHEMA "org.gnome.desktop.security-framework"
#define PASS_PHRASE               "n6x6myibEAvfN9vIDDPQi+iCoE7yTuHP//eC195+g7w="

struct _CcSecurityFrameworkPanel
{
  CcPanel    parent_instance;
  GThread   *thr;
  GDBusProxy *dbus_proxy;
  GtkWidget *ghub_section;
  GtkWidget *gauth_section;
  GtkWidget *gpms_button;
  GtkWidget *gcontroller_button;
  GtkWidget *apps_button;
  GtkWidget *gagent_button;
  GtkWidget *darea_11;
  GtkWidget *darea_12;
  GtkWidget *darea_13;
  GtkWidget *darea_14;
  GtkWidget *darea_15;
  GtkWidget *darea_22;
  GtkWidget *darea_24;
  GtkWidget *darea_33;
  GtkWidget *darea_51;
  GtkWidget *darea_52;
  GtkWidget *darea_53;
  GtkWidget *darea_54;
  GtkWidget *darea_55;
  GtkWidget *log_label;
  GtkWidget *application_layer_label;
  GtkWidget *lsf_layer_label;
  GtkWidget *server_layer_label;
  GtkWidget *menu[CELL_NUM];
  GtkWidget *control_center_image;
  GtkWidget *ghub_image;
  GtkWidget *gauth_image;
  GtkWidget *gcontroller_image;
  GtkWidget *gagent_image;
  GtkWidget *gpms_image;
  GtkWidget *apps_image;
  GtkWidget *lsf_image;
  GtkWidget *sound_wave;
  GtkWidget *message_label[CELL_NUM];
  GtkWidget *module_info_box;
  GtkWidget *popover[CELL_NUM];
  GtkWidget *cover_popover;
  GtkWidget *cover_label;
  GtkWidget *cover_sub_label;
  GtkWidget *cover_box;
  gboolean   activated[CELL_NUM];
  gboolean   animating;
  guint      event_source_tag[EVENTS_NUM];
  gchar     *log_message[LOG_BUF];
  gchar     *from_log;
  gchar     *to_log;
  FILE      *fp;
  gint       events;
  gint       scene;
  gint       scene_cnt;
  gint       target_cell;
  gint       log_start;
  gint       log_end;
  gint       log_cnt;
  gint       from;
  gint       to;
};

G_DEFINE_TYPE (CcSecurityFrameworkPanel, cc_security_framework_panel, CC_TYPE_PANEL)

static void do_drawing           (GtkWidget *, cairo_t *, gint, gint, gint, gint);
static void menu_item_selected   (GtkWidget *, GdkEvent *, gpointer);
static void set_line_color       (cairo_t *, gint);

static void
enqueue_log_label (CcSecurityFrameworkPanel *self, char *new_log_message)
{
  self->log_end = (self->log_end+1)%LOG_BUF;

  if (self->log_cnt == LOG_BUF)
  {
    g_free (self->log_message[self->log_start]);
    self->log_start = (self->log_start+1)%LOG_BUF;
  }
  else
  {
    self->log_cnt++;
  }
  self->log_message[self->log_end] = g_strdup (new_log_message);
}

static void
renew_log_label (CcSecurityFrameworkPanel *self)
{
  int i;
  gchar *logs = "";

  for (i = 0; i != self->log_cnt; i++)
  {
    logs = g_strjoin ("\t", logs, self->log_message[((self->log_start)+i)%LOG_BUF], "\n", NULL);
  }

  gtk_label_set_text (GTK_LABEL (self->log_label), logs);
}

static void
draw_vertical_bar (GtkWidget *widget,
                   cairo_t   *cr,
                   gpointer   user_data)
{
  double dashed[] = { 3.0 };

  set_line_color (cr, COLOR_BLACK);
  cairo_set_line_width (cr, 2.0);
  cairo_set_dash (cr, dashed, 1, 0);

  cairo_move_to (cr, 48, 0);
  cairo_line_to (cr, 48, 73);
  cairo_stroke (cr);
}

static void
draw_conn_cc_ghub (GtkWidget *widget,
                   cairo_t   *cr,
                   gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  int color;

  if (self->activated[GHUB_CELL])
  {
    color = COLOR_GREEN;
  }
  else
  {
    color = COLOR_NONE;
  }
  do_drawing (widget, cr, DIRECTION_CC_GHUB, color, self->scene, self->scene_cnt);
}

static void
draw_conn_ghub_gauth (GtkWidget *widget,
                      cairo_t   *cr,
                      gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  int color;

  if (self->activated[GHUB_CELL] && self->activated[GAUTH_CELL])
  {
    color = COLOR_GREEN;
  }
  else
  {
    color = COLOR_NONE;
  }
  do_drawing (widget, cr, DIRECTION_GHUB_GAUTH, color, self->scene, self->scene_cnt);
}

static void
draw_conn_ghub_gctrl (GtkWidget *widget,
                      cairo_t   *cr,
                      gpointer   user_data)
{
  int color;
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;

  if (self->activated[GHUB_CELL] && self->activated[GCTRL_CELL])
  {
    color = COLOR_GREEN;
  } 
  else
  {
    color = COLOR_NONE;
  }
  do_drawing (widget, cr, DIRECTION_GHUB_GCTRL, color, self->scene, self->scene_cnt);
}

static void
draw_conn_ghub_gagent (GtkWidget *widget,
                       cairo_t   *cr,
                       gpointer   user_data)
{
  int color;
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;

  if (self->activated[GHUB_CELL] && self->activated[GAGENT_CELL])
  {
    color = COLOR_GREEN;
  }
  else
  {
    color = COLOR_NONE;
  }
  do_drawing (widget, cr, DIRECTION_GHUB_GAGENT, color, self->scene, self->scene_cnt);
}

static void
draw_conn_ghub_apps (GtkWidget *widget,
                     cairo_t   *cr,
                     gpointer   user_data)
{
  int color;
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;

  if (self->activated[GHUB_CELL] && self->activated[APPS_CELL])
  {
    color = COLOR_GREEN;
  }
  else
  {
    color = COLOR_NONE;
  }
  do_drawing (widget, cr, DIRECTION_GHUB_APPS, color, self->scene, self->scene_cnt);
}

static void
draw_conn_gagent_gpms (GtkWidget *widget,
                       cairo_t   *cr,
                       gpointer   user_data)
{
  int color;
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;

  if (self->activated[GAGENT_CELL])
  {
    color = COLOR_BLUE;
  }
  else
  {
    color = COLOR_NONE;
  }
  do_drawing (widget, cr, DIRECTION_GAGENT_GPMS, color, self->scene, self->scene_cnt);
}

static void
log_label_clicked (GtkWidget      *widget,
                   GdkEventButton *event,
                   gpointer        user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;

  if (event->type == GDK_2BUTTON_PRESS)
  {
    GtkWidget* log_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (log_window), "GHub Log");
    gtk_window_set_default_size (GTK_WINDOW (log_window), 400, 400);
    gtk_window_set_position (GTK_WINDOW (log_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_show (log_window);
  }
}

static void
gpms_cell_clicked (GtkWidget      *widget,
                   GdkEventButton *event,
                   gpointer        user_data)
{
  pid_t pid;
  char *argv[] = { "gooroom-browser", GPMS_DOMAIN, NULL };

  if (event->button == GDK_BUTTON_PRIMARY)
  {
    pid = fork ();
    if (pid == 0)
    {
      execv ("/usr/bin/gooroom-browser", argv);
      exit (EXIT_SUCCESS);
    }
  }
}

static void
gctrl_cell_clicked (GtkWidget      *widget,
                    GdkEventButton *event,
                    gpointer        user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  
  if (event->button == GDK_BUTTON_PRIMARY)
  {
    gtk_menu_popup_at_pointer (GTK_MENU (self->menu[GCTRL_CELL]), NULL);
  }
}

static void
gagent_cell_clicked (GtkWidget      *widget,
                     GdkEventButton *event,
                     gpointer        user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  

  if (event->button == GDK_BUTTON_PRIMARY)
  {
    gtk_menu_popup_at_pointer (GTK_MENU (self->menu[GAGENT_CELL]), NULL);
  }
}

static void
apps_cell_clicked (GtkWidget      *widget,
                   GdkEventButton *event,
                   gpointer        user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;

  if (event->button == GDK_BUTTON_PRIMARY)
  {
    gtk_menu_popup_at_pointer (GTK_MENU (self->menu[APPS_CELL]), NULL);
  }
}

static void
dbus_message_sender (gpointer arg_p)
{
  int arg = GPOINTER_TO_INT (arg_p);
  GVariant *ret;
  //GDBusMessage *ret;
  //char ret_str[4096];
  gchar *ret_str;
  GDBusConnection *conn = NULL;
  GDBusProxy *dbus_proxy = NULL;
  conn = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);

  switch (arg)
  {
    case SET_CONFIG:
      g_print ("DBUS: set_config\n");
      break;
    case UNSET_CONFIG:
      g_print ("DBUS: unset_config\n");
      break;
    case LAUNCH_GAGENT:
      g_print ("DBUS: launch_gagent\n");
      break;
    case KILL_GAGENT:
      g_print ("DBUS: kill_gagent\n");
      break;
    case LAUNCH_APPS:
      g_print ("DBUS: launch_apps\n");

      dbus_proxy = g_dbus_proxy_new_sync (conn,
                                          G_DBUS_PROXY_FLAGS_NONE,
                                          NULL,
                                          "kr.gooroom.bhsdGHub",
                                          "/kr/gooroom/bhsdGHubObject",
                                          "kr.gooroom.bhsdGHubInterface",
                                          NULL,
                                          NULL);
      ret = g_dbus_proxy_call_sync (dbus_proxy,
                                    "EXECUTE",
                                    NULL,
                                    G_DBUS_CALL_FLAGS_NONE,
                                    -1,
                                    NULL,
                                    NULL);
      g_variant_get (ret, "(s)", &ret_str); // TODO
      g_print ("%s\n", ret_str);
      break;
    case KILL_APPS:
      g_print ("DBUS: kill_apps\n");
      break;
  }

}

static void
gctrl_menu_handler (GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   user_data)
{
  const gchar *selection = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));
  GThread *thr;

  if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (widget))) 
  {
    thr = g_thread_new (NULL, dbus_message_sender, GINT_TO_POINTER (SET_CONFIG));
  }
  else if (!g_strcmp0 (selection, "Off"))
  {
    thr = g_thread_new (NULL, dbus_message_sender, GINT_TO_POINTER (UNSET_CONFIG));
  }
}

static void
apps_menu_handler (GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   user_data)
{
  const gchar *selection = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));
  GThread *thr;

  if (!g_strcmp0 (selection, "Kill"))
  {
    thr = g_thread_new (NULL, dbus_message_sender, GINT_TO_POINTER (KILL_APPS));
  }
  else if (!g_strcmp0 (selection, "Launch"))
  {
    thr = g_thread_new (NULL, dbus_message_sender, GINT_TO_POINTER (LAUNCH_APPS));
  }
}

static void
gagent_menu_handler (GtkWidget *widget,
                     GdkEvent  *event,
                     gpointer   user_data)
{
  const gchar *selection = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));
  GThread   *thr;

  if (!g_strcmp0 (selection, "Kill"))
  {
    thr = g_thread_new (NULL, dbus_message_sender, GINT_TO_POINTER (KILL_GAGENT));
  }
  else if (!g_strcmp0 (selection, "Launch"))
  {
    thr = g_thread_new (NULL, dbus_message_sender, GINT_TO_POINTER (LAUNCH_GAGENT));
  }
}

static void
set_line_color (cairo_t *cr,
                gint     color)
{
  switch (color)
  {
    case COLOR_BLACK:
      cairo_set_source_rgba (cr, 0.7, 0.7, 0.7, 1);
      break;
    case COLOR_RED:
      cairo_set_source_rgba (cr, 0.71, 0.06, 0.15, 1);
      break;
    case COLOR_GREEN:
      cairo_set_source_rgba (cr, 0.44, 0.74, 0.12, 1);
      break;
    case COLOR_BLUE:
      cairo_set_source_rgba (cr, 0.05, 0.39, 0.82, 1);
      break;
    case COLOR_YELLOW:
      cairo_set_source_rgba (cr, 0.9, 0.9, 0.1, 1);
      break;
    default:
      cairo_set_source_rgba (cr, 0.64, 0.64, 0.64, 1);
      break;
  }
}

static void
set_menu_items (CcSecurityFrameworkPanel *object,
                gint                      module)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) object;
  GtkWidget *menu;
  GtkWidget *sub_menu;
  GtkWidget *menu_item;
  GSList *conf_group = NULL;

  if (module == GCTRL_CELL)
  {
    menu = self->menu[GCTRL_CELL];
    menu_item = gtk_menu_item_new_with_label ("Configuration Management");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 0, 1);
    sub_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), sub_menu);
    menu_item = gtk_radio_menu_item_new_with_label (conf_group, "On");
    conf_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menu_item));
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (gctrl_menu_handler),
                      self);
    gtk_menu_attach (GTK_MENU (sub_menu), menu_item, 0, 1, 0, 1);
    menu_item = gtk_radio_menu_item_new_with_label (conf_group, "Off");
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (gctrl_menu_handler),
                      self);
    gtk_menu_attach (GTK_MENU (sub_menu), menu_item, 0, 1, 1, 2);
    gtk_widget_show_all (menu);
  }
  else if (module == APPS_CELL)
  {
    menu = self->menu[module];
    menu_item = gtk_menu_item_new_with_label ("Launch");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 0, 1);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (apps_menu_handler),
                      NULL);
    menu_item = gtk_menu_item_new_with_label ("Kill");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 1, 2);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (apps_menu_handler),
                      NULL);
    gtk_widget_show_all (menu);
  }
  else if (module == GAGENT_CELL)
  {
    menu = self->menu[module];
    menu_item = gtk_menu_item_new_with_label ("Launch");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 0, 1);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (gagent_menu_handler),
                      NULL);
    menu_item = gtk_menu_item_new_with_label ("Kill");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 1, 2);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (gagent_menu_handler),
                      NULL);
    gtk_widget_show_all (menu);
  }
}

static void
set_modules_opacity (CcSecurityFrameworkPanel *self)
{
  if (self->activated[GHUB_CELL])
  {
    gtk_widget_set_opacity (self->ghub_section, 1.0);
  }
  else
  {
    gtk_widget_set_opacity (self->ghub_section, 0.3);
  }

  if (self->activated[GAUTH_CELL])
  {
    gtk_widget_set_opacity (self->gauth_section, 1.0);
  }
  else
  {
    gtk_widget_set_opacity (self->gauth_section, 0.3);
  }

  if (self->activated[GCTRL_CELL])
  {
    gtk_widget_set_opacity (self->gcontroller_button, 1.0);
  }
  else
  {
    gtk_widget_set_opacity (self->gcontroller_button, 0.3);
  }

  if (self->activated[GAGENT_CELL])
  {
    gtk_widget_set_opacity (self->gagent_button, 1.0);
  }
  else
  {
    gtk_widget_set_opacity (self->gagent_button, 0.3);
  }

  if (self->activated[APPS_CELL])
  {
    gtk_widget_set_opacity (self->apps_button, 1.0);
  }
  else
  {
    gtk_widget_set_opacity (self->apps_button, 0.3);
  }
}

static void
draw_lines (CcSecurityFrameworkPanel *self)
{
  gtk_widget_queue_draw (self->darea_12);
  gtk_widget_queue_draw (self->darea_22);
  gtk_widget_queue_draw (self->darea_24);
  gtk_widget_queue_draw (self->darea_33);
  gtk_widget_queue_draw (self->darea_14);
  gtk_widget_queue_draw (self->darea_53);
}

static void
get_modules_state (gpointer *self_p)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) self_p;
  int i;

  sleep (1);
  g_print ("DBUS: get_module state\n");
  for (i = 0; i < CELL_NUM; i++)
  {
    self->activated[i] = TRUE;
  }
  set_modules_opacity (self);
}

static void
do_drawing (GtkWidget *widget,
            cairo_t   *cr,
            gint       direction,
            gint       color,
            gint       scene,
            gint       scene_cnt)
{
  int i;
  int reverse = 0;
  gboolean scope = FALSE;
  gboolean vert_bar = FALSE;
  int mid_right[2] = { 78, 35 };
  int mid_left[2] = { 18, 35 };
  int mid_up[2] = { 46, 18 };
  int mid_down[2] = { 46, 60 };
  int mid_mid[2] = { 46, 35 };
  int top_left[2] = { 18, 18 };
  int bottom_right[2] = { 78, 55 };
  int top_right[2] = { 78, 18 };
  int bottom_left[2] = { 0, 70 };
  double dashed[] = { 3.0 };
  int ht = HT;
  int vt = VT;
  int xpos;
  int ypos;

  cairo_set_line_width (cr, 2.0);
  set_line_color (cr, color);

  switch (scene)
  {
    case SCENE_METHOD_CALL:
      if (STARTING_BLINK_CNT < scene_cnt
          && scene_cnt < STARTING_BLINK_CNT+MOVING_CNT)
      {
        scope = TRUE;
      }
      break;
    case SCENE_METHOD_CALL_REV:
      if (STARTING_BLINK_CNT < scene_cnt
          && scene_cnt < STARTING_BLINK_CNT+MOVING_CNT)
      {
        scope = TRUE;
        reverse = 1;
      }
      break;
    case SCENE_GHUB_BROADCAST:
      if (SCENE_GHUB_BROADCAST_GHUB_BLINKING < scene_cnt
          && scene_cnt < SCENE_GHUB_BROADCAST_GHUB_BLINKING+MOVING_CNT)
      {
        scope = TRUE;
      }
      break;
  }

  switch (direction)
  {
    case DIRECTION_GHUB_GAUTH:
      xpos = mid_down[XPOS];
      ypos = mid_down[YPOS];
      ht = 0;
      vt *= (-1);
      break;
    case DIRECTION_GHUB_GCTRL:
      xpos = mid_up[XPOS];
      ypos = mid_up[YPOS];
      ht = 0;
      break;
    case DIRECTION_GHUB_GAGENT:
      switch (scene)
      {
        case SCENE_POLICY_RELOAD:
          if (SCENE_POLICY_RELOAD_GAGENT_BLINKING < scene_cnt
              && scene_cnt < SCENE_POLICY_RELOAD_GAGENT_BLINKING+MOVING_CNT)
          {
            scope = TRUE;
            reverse = 1;
          }
          break;
      }
      xpos = mid_left[XPOS];
      ypos = mid_left[YPOS];
      vt = 0;
      break;
    case DIRECTION_GAGENT_GPMS:
      switch (scene)
      {
        case SCENE_POLICY_RELOAD:
          if (SCENE_POLICY_RELOAD_GPMS_BLINKING < scene_cnt
              && scene_cnt < SCENE_POLICY_RELOAD_GPMS_BLINKING+MOVING_CNT)
          {
            scope = TRUE;
          }
          break;
      }
      xpos = mid_right[XPOS];
      ypos = mid_right[YPOS];
      ht *= (-1);
      vt = 0;
      vert_bar = TRUE;
      break;
    case DIRECTION_GHUB_APPS:
      xpos = top_right[XPOS];
      ypos = top_right[YPOS];
      ht *= (-1);
      vert_bar = TRUE;
      break;
    case DIRECTION_CC_GHUB:
      xpos = top_left[XPOS];
      ypos = top_left[YPOS];
      vert_bar = TRUE;
      break;
  }
  for (i = 0; i < 4; i++)
  {
    if (scope)
    {
      if (((scene_cnt+i+reverse)%2))
      {
        set_line_color (cr, color);
        cairo_arc (cr, xpos+(ht*i), ypos+(vt*i), RADIUS_SMALL, 0, 2*M_PI);
      }
      else
      {
        set_line_color (cr, COLOR_YELLOW);
        cairo_arc (cr, xpos+(ht*i), ypos+(vt*i), RADIUS_LARGE, 0, 2*M_PI);
      }
    }
    else
    {
      set_line_color (cr, color);
      cairo_arc (cr, xpos+(ht*i), ypos+(vt*i), RADIUS_MEDIUM, 0, 2*M_PI);
    }
    cairo_fill (cr);
  }
  if (vert_bar)
  {
    set_line_color (cr, COLOR_BLACK);
    cairo_set_dash (cr, dashed, 1, 0);
    cairo_move_to (cr, 48, 0);
    cairo_line_to (cr, 48, 73);
    cairo_stroke (cr);
  }
}

static void
scene_handler (CcSecurityFrameworkPanel *self)
{
  if (self->scene == SCENE_IDLE)
  {
    g_print ("IDLE\n");
    self->animating = FALSE;
    self->scene_cnt = 0;
  }
  else
  {
    switch (self->scene_cnt)
    {
      case 0:
        self->animating = TRUE;
        switch (self->scene)
        {
          // TODO: DELETE_START
          case SCENE_PRESENTING:
            gtk_widget_set_opacity (self->cover_popover, 0.75);
            gtk_image_set_from_file (GTK_IMAGE (self->lsf_image), LSF_IMG);
            gtk_widget_show_all (self->cover_popover);
            gtk_image_set_from_file (GTK_IMAGE (self->sound_wave), "images/sound-wave.gif");
            g_spawn_command_line_async ("/usr/bin/aplay /home/haru/policy-reload.wav", NULL);
            break;
            // DELETE_END
          case SCENE_POLICY_RELOAD:
            gtk_label_set_text (GTK_LABEL (self->message_label[GPMS_CELL]),
                                "<b><span font='10' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[GPMS_CELL]), TRUE);
            enqueue_log_label (self, "GPMS: 보안 정책 변경");
            gtk_popover_popup (GTK_POPOVER (self->popover[GPMS_CELL]));
            break;
          case SCENE_METHOD_CALL:
          case SCENE_METHOD_CALL_REV:
            enqueue_log_label (self, self->from_log);
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_label_set_text (GTK_LABEL (self->message_label[GHUB_CELL]),
                                "<b><span font='10' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 전달</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[GHUB_CELL]), TRUE);
            gtk_popover_popup (GTK_POPOVER (self->popover[GHUB_CELL]));
            enqueue_log_label (self, "GHUB: 보안 정책 변경 전달");
            break;
        }
        renew_log_label (self);
        break;
      case 1: case 3: case 5:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG);
            break;
          case SCENE_METHOD_CALL:
          case SCENE_METHOD_CALL_REV:
            switch (self->from)
            {
              case CC_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG);
                break;
              case GHUB_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG);
                break;
              case GAUTH_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG);
                break;
              case GCTRL_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG);
                break;
              case GAGENT_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG);
                break;
              case APPS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG);
                break;
              case GPMS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG);
                break;
            }
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG);
            break;
        }
        break;
      case 2: case 4:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG_SMALL);
            break;
          case SCENE_METHOD_CALL:
          case SCENE_METHOD_CALL_REV:
            switch (self->from)
            {
              case CC_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG_SMALL);
                break;
              case GHUB_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG_SMALL);
                break;
              case GAUTH_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG_SMALL);
                break;
              case GCTRL_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG_SMALL);
                break;
              case GAGENT_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG_SMALL);
                break;
              case APPS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG_SMALL);
                break;
              case GPMS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG_SMALL);
                break;
            }
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG_SMALL);
            break;
        }
        break;
      case 6: case 7: case 8: case 9: case 10:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            if (self->scene_cnt == 6)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GPMS_CELL]));
            }
            gtk_widget_queue_draw (self->darea_53);
            break;
          case SCENE_METHOD_CALL:
            switch (self->from)
            {
              case CC_CELL:
                gtk_widget_queue_draw (self->darea_12);
                break;
              case GAUTH_CELL:
                gtk_widget_queue_draw (self->darea_22);
                break;
              case GCTRL_CELL:
                gtk_widget_queue_draw (self->darea_24);
                break;
              case GAGENT_CELL:
                gtk_widget_queue_draw (self->darea_33);
                break;
              case APPS_CELL:
                gtk_widget_queue_draw (self->darea_14);
                break;
              case GPMS_CELL:
                gtk_widget_queue_draw (self->darea_53);
                break;
            }
            break;
          case SCENE_METHOD_CALL_REV:
            switch (self->to)
            {
              case CC_CELL:
                gtk_widget_queue_draw (self->darea_12);
                break;
              case GAUTH_CELL:
                gtk_widget_queue_draw (self->darea_22);
                break;
              case GCTRL_CELL:
                gtk_widget_queue_draw (self->darea_24);
                break;
              case GAGENT_CELL:
                gtk_widget_queue_draw (self->darea_33);
                break;
              case APPS_CELL:
                gtk_widget_queue_draw (self->darea_14);
                break;
              case GPMS_CELL:
                gtk_widget_queue_draw (self->darea_53);
                break;
            }
            break;
          case SCENE_GHUB_BROADCAST:
            if (self->scene_cnt == 6)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GHUB_CELL]));
            }
            gtk_widget_queue_draw (self->darea_12);
            gtk_widget_queue_draw (self->darea_22);
            gtk_widget_queue_draw (self->darea_24);
            gtk_widget_queue_draw (self->darea_14);
            break;
        }
        break;
      case 11: case 13: case 15:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            if (self->scene_cnt == 11)
            {
              gtk_label_set_text (GTK_LABEL (self->message_label[GAGENT_CELL]),
                                  "<b><span font='10' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 반영</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GAGENT_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[GAGENT_CELL]));
              enqueue_log_label (self, "GAGENT: 보안 정책 변경 반영");
              renew_log_label (self);
            }
            gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG);
            break;
          case SCENE_METHOD_CALL:
          case SCENE_METHOD_CALL_REV:
            if (self->scene_cnt == 11)
            {
              enqueue_log_label (self, self->to_log);
              renew_log_label (self);
            }
            switch (self->to)
            {
              case CC_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG);
                break;
              case GHUB_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG);
                break;
              case GAUTH_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG);
                break;
              case GCTRL_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG);
                break;
              case GAGENT_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG);
                break;
              case APPS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG);
                break;
              case GPMS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG);
                break;
            }
            break;
          case SCENE_GHUB_BROADCAST:
            if (self->scene_cnt == 11)
            {
              gtk_label_set_text (GTK_LABEL (self->message_label[CC_CELL]),
                                  "<b><span font='10' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 반영</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[CC_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[CC_CELL]));
              gtk_label_set_text (GTK_LABEL (self->message_label[GAUTH_CELL]),
                                  "<b><span font='10' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 반영</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GAUTH_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[GAUTH_CELL]));
              gtk_label_set_text (GTK_LABEL (self->message_label[GCTRL_CELL]),
                                  "<b><span font='10' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 반영</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GCTRL_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[GCTRL_CELL]));
              gtk_label_set_text (GTK_LABEL (self->message_label[APPS_CELL]),
                                  "<b><span font='10' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 반영</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[APPS_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[APPS_CELL]));
              enqueue_log_label (self, "CC: 보안 정책 반영");
              enqueue_log_label (self, "GAUTH: 보안 정책 반영");
              enqueue_log_label (self, "GCTRL: 보안 정책 반영");
              enqueue_log_label (self, "APPS: 보안 정책 반영");
            }
            gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG);
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG);
            gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG);
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG);
            break;
        }
        renew_log_label (self);
        break;
      case 12: case 14:
        switch (self->scene)
        {
          case SCENE_PRESENTING:
            if (self->scene_cnt == 14)
            {
              gtk_widget_hide (self->cover_popover);
              gtk_image_clear (GTK_IMAGE (self->lsf_image));
              self->scene = SCENE_POLICY_RELOAD;
              self->scene_cnt = SCENE_END;
            }
            break;
          case SCENE_POLICY_RELOAD:
            gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG_SMALL);
            break;
          case SCENE_METHOD_CALL:
          case SCENE_METHOD_CALL_REV:
            switch (self->to)
            {
              case CC_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG_SMALL);
                break;
              case GHUB_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG_SMALL);
                break;
              case GAUTH_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG_SMALL);
                break;
              case GCTRL_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG_SMALL);
                break;
              case GAGENT_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG_SMALL);
                break;
              case APPS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG_SMALL);
                break;
              case GPMS_CELL:
                gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG_SMALL);
                break;
            }
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG_SMALL);
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG_SMALL);
            gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG_SMALL);
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG_SMALL);
            break;
        }
        break;
      case 16: 
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            gtk_popover_popdown (GTK_POPOVER (self->popover[GAGENT_CELL]));
            self->scene = SCENE_METHOD_CALL;
            self->from = GAGENT_CELL;
            self->to = GHUB_CELL;
            self->from_log = g_strdup ("GAGENT: 보안 정책 변경 전달");
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_popover_popdown (GTK_POPOVER (self->popover[CC_CELL]));
            gtk_popover_popdown (GTK_POPOVER (self->popover[GAUTH_CELL]));
            gtk_popover_popdown (GTK_POPOVER (self->popover[GCTRL_CELL]));
            gtk_popover_popdown (GTK_POPOVER (self->popover[APPS_CELL]));
            break;
          default:
            self->scene = SCENE_IDLE;
            self->animating = FALSE;
            break;
        }
        self->scene_cnt = SCENE_END;
        break;
    }
    self->scene_cnt = (self->scene_cnt+1)%SCENE_CNT;
  }
}

static int
get_cell (const char *dbus_name)
{
  if (!g_strcmp0 (dbus_name, CC_DBUS_NAME))
  {
    return CC_CELL;
  }
  else if (!g_strcmp0 (dbus_name, GHUB_DBUS_NAME))
  {
    return GHUB_CELL;
  }
  else if (!g_strcmp0 (dbus_name, GAUTH_DBUS_NAME))
  {
    return GAUTH_CELL;
  }
  else if (!g_strcmp0 (dbus_name, GCTRL_DBUS_NAME))
  {
    return GCTRL_CELL;
  }
  else if (!g_strcmp0 (dbus_name, GAGENT_DBUS_NAME))
  {
    return GAGENT_CELL;
  }
  else if (!g_strcmp0 (dbus_name, GPMS_NAME))
  {
    return GPMS_CELL;
  }
  else if (!g_strcmp0 (dbus_name, APPS_DBUS_NAME))
  {
    return APPS_CELL;
  }
  else
  {
    return -1;
  }
}

static gchar *
get_module_name (int cell)
{
  if (cell == CC_CELL)
  {
    return "Control Center";
  }
  else if (cell == GHUB_CELL)
  {
    return "GHUB";
  }
  else if (cell == GAUTH_CELL)
  {
    return "GAUTH";
  }
  else if (cell == GCTRL_CELL)
  {
    return "GCTRL";
  }
  else if (cell == GAGENT_CELL)
  {
    return "GAGENT";
  }
  else if (cell == APPS_CELL)
  {
    return "APPS";
  }
  else if (cell == GPMS_CELL)
  {
    return "GPMS";
  }
  else
  {
    return "";
  }
}

static void
get_scene (CcSecurityFrameworkPanel *self)
{
  gchar **log_str;
  gchar **args;
  char buf[4096];

  if (self->scene == SCENE_IDLE) 
  {
    fgets (buf, 4096, self->fp);
    log_str = g_strsplit (buf, " ", 0);
    args = g_strsplit (log_str[2], ",", 0);
    self->from = get_cell (args[DMSG_FROM]);
    self->to = get_cell (args[DMSG_TO]);
    g_print ("sequence_num: %s\n", args[DMSG_SEQ]);
    g_print ("method: %s\n", args[DMSG_METHOD]);
    g_print ("glyph: %s\n", args[DMSG_GLYPH]);
    g_print ("func: %s\n", args[DMSG_FUNC]);
    g_print ("from: %d\n", self->from);
    g_print ("to: %d\n", self->to);

    if (!g_strcmp0 (args[DMSG_GLYPH], "O"))
    {
      self->scene = SCENE_PRESENTING;
    }
    else 
    {
      self->from_log = g_strconcat (get_module_name (self->from), ": ", args[DMSG_METHOD], NULL);
      if (self->from == GHUB_CELL)
      {
        self->scene = SCENE_METHOD_CALL_REV;
      }
      else
      {
        self->scene = SCENE_METHOD_CALL;
      }
    }
    if (args)
    {
      g_strfreev (args);
    }
    if (log_str)
    {
      g_strfreev (log_str);
    }
  }
}

static gboolean
time_handler (GObject *object)
{
  CcSecurityFrameworkPanel *self = CC_SECURITY_FRAMEWORK_PANEL (object);

  if (!self->animating)
  {
    set_modules_opacity (self);
    get_scene (self);
  }
  scene_handler (self);

  return TRUE;
}

static gboolean
time_handler2 (GObject *object)
{
  CcSecurityFrameworkPanel *self = CC_SECURITY_FRAMEWORK_PANEL (object);

  self->thr = g_thread_new (NULL, get_modules_state, (gpointer) self);

  return TRUE;
}

static const char *
cc_security_framework_panel_get_help_uri (CcPanel *self)
{
  return "help:gnome-help/security-framework";
}

static void
cc_security_framework_panel_dispose (GObject *object)
{
  CcSecurityFrameworkPanel *security_framework_panel = CC_SECURITY_FRAMEWORK_PANEL (object);
  int i;

  if (security_framework_panel->events)
  {
    for (i = 0; i < security_framework_panel->events; i++)
    {
      g_source_remove (security_framework_panel->event_source_tag[i]);
    }
    security_framework_panel->events = 0;
  }

  if (security_framework_panel->fp)
  {
    fclose (security_framework_panel->fp);
    security_framework_panel->fp = NULL;
  }

  G_OBJECT_CLASS (cc_security_framework_panel_parent_class)->dispose (object);
}

static void
cc_security_framework_panel_constructed (GObject *object)
{
  CcSecurityFrameworkPanel *self = CC_SECURITY_FRAMEWORK_PANEL (object);

  self->event_source_tag[self->events++] = g_timeout_add (10, (GSourceFunc) time_handler, (gpointer) object);
  self->event_source_tag[self->events++] = g_timeout_add (1000, (GSourceFunc) time_handler2, (gpointer) object);
}

static void
cc_security_framework_panel_class_init (CcSecurityFrameworkPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = cc_security_framework_panel_dispose;
  object_class->constructed = cc_security_framework_panel_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/security-framework/security-framework.ui");
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, ghub_section);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gauth_section);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gpms_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gcontroller_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gagent_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, apps_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_11);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_12);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_13);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_14);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_15);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_22);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_24);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_33);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_51);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_52);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_53);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_54);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_55);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, application_layer_label);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, lsf_layer_label);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, server_layer_label);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, control_center_image);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, ghub_image);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gauth_image);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gcontroller_image);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gpms_image);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gagent_image);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, apps_image);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, module_info_box);
}

static void
cc_security_framework_panel_init (CcSecurityFrameworkPanel *self)
{
  int module;
  GtkWidget *event_box;
  PangoAttrList *pg_attr_list; 
  PangoAttribute *pg_attr;
  GdkRGBA cover_color;
  GdkRGBA application_layer_color;

  g_resources_register (cc_security_framework_get_resource ());

  gtk_widget_init_template (GTK_WIDGET (self));
  gdk_rgba_parse (&cover_color, "#000000");
  gdk_rgba_parse (&application_layer_color, "rgba(255,0,0,0.5)");

  get_modules_state (self);

  self->fp = fopen ("/home/haru/haru_project/lsf/lsf_logger/logs/message-2020-06-02.log", "r");
  self->events = 0;
  self->log_start = 0;
  self->log_end = -1;
  self->log_cnt = 0;

  self->scene = SCENE_IDLE;
  draw_lines (self);
  self->target_cell = GAGENT_CELL;

  event_box = gtk_event_box_new ();
  gtk_event_box_set_above_child (GTK_EVENT_BOX (event_box), FALSE);

  self->menu[GAGENT_CELL] = gtk_menu_new ();
  set_menu_items (self, GAGENT_CELL);

  self->menu[APPS_CELL] = gtk_menu_new ();
  set_menu_items (self, APPS_CELL);

  self->menu[GCTRL_CELL] = gtk_menu_new ();
  set_menu_items (self, GCTRL_CELL);

  self->popover[CC_CELL] = gtk_popover_new (self->control_center_image);
  self->popover[GHUB_CELL] = gtk_popover_new (self->ghub_image);
  self->popover[GAUTH_CELL] = gtk_popover_new (self->gauth_image);
  self->popover[GCTRL_CELL] = gtk_popover_new (self->gcontroller_image);
  self->popover[GAGENT_CELL] = gtk_popover_new (self->gagent_image);
  self->popover[APPS_CELL] = gtk_popover_new (self->apps_image);
  self->popover[GPMS_CELL] = gtk_popover_new (self->gpms_image);

  for (module = 0; module < CELL_NUM; module++)
  {
    self->message_label[module] = gtk_label_new ("");
    gtk_widget_show (self->message_label[module]);
    gtk_container_add (GTK_CONTAINER (self->popover[module]), self->message_label[module]);
    gtk_popover_set_modal (GTK_POPOVER (self->popover[module]), FALSE);
  }

  self->lsf_image = gtk_image_new ();
  gtk_widget_set_size_request (self->lsf_image, -1, 45);
  gtk_widget_set_margin_bottom (self->lsf_image, 90);
  gtk_widget_set_vexpand (self->lsf_image, FALSE);
  gtk_box_pack_start (GTK_BOX (self->module_info_box), self->lsf_image, FALSE, FALSE, 0);

  self->log_label = gtk_label_new ("");
  gtk_label_set_justify (GTK_LABEL (self->log_label), GTK_JUSTIFY_LEFT);
  gtk_container_add (GTK_CONTAINER (event_box), self->log_label);
  gtk_box_pack_start (GTK_BOX (self->module_info_box), event_box, FALSE, FALSE, 0);

  pg_attr_list = pango_attr_list_new ();
  pg_attr = pango_attr_size_new_absolute (12*PANGO_SCALE);
  pango_attr_list_insert (pg_attr_list, pg_attr);
  gtk_label_set_attributes (GTK_LABEL (self->log_label), pg_attr_list);
  gtk_label_set_width_chars (GTK_LABEL (self->log_label), 30);
  gtk_label_set_xalign (GTK_LABEL (self->log_label), 0);
  gtk_label_set_yalign (GTK_LABEL (self->log_label), 0);

  gtk_label_set_text (GTK_LABEL (self->application_layer_label),
                                "<b><span font='12' font-weight='bold' foreground='#3a3c84'>       보안\n애플리케이션\n</span></b>");
  gtk_label_set_use_markup (GTK_LABEL (self->application_layer_label), TRUE);
  gtk_label_set_text (GTK_LABEL (self->lsf_layer_label),
                                "<b><span font='12' font-weight='bold' foreground='#3a3c84'>        경량\n보안프레임워크\n</span></b>");
  gtk_label_set_use_markup (GTK_LABEL (self->lsf_layer_label), TRUE);
  gtk_label_set_text (GTK_LABEL (self->server_layer_label),
                                "<b><span font='12' font-weight='bold' foreground='#3a3c84'>  단말운용\n관리솔루션\n</span></b>");
  gtk_label_set_use_markup (GTK_LABEL (self->server_layer_label), TRUE);


  gtk_widget_show_all (self->module_info_box);

  g_signal_connect (G_OBJECT (self->darea_11),
                    "draw",
                    G_CALLBACK (draw_vertical_bar),
                    self);
  g_signal_connect (G_OBJECT (self->darea_13),
                    "draw",
                    G_CALLBACK (draw_vertical_bar),
                    self);
  g_signal_connect (G_OBJECT (self->darea_15),
                    "draw",
                    G_CALLBACK (draw_vertical_bar),
                    self);
  g_signal_connect (G_OBJECT (self->darea_51),
                    "draw",
                    G_CALLBACK (draw_vertical_bar),
                    self);
  g_signal_connect (G_OBJECT (self->darea_52),
                    "draw",
                    G_CALLBACK (draw_vertical_bar),
                    self);
  g_signal_connect (G_OBJECT (self->darea_54),
                    "draw",
                    G_CALLBACK (draw_vertical_bar),
                    self);
  g_signal_connect (G_OBJECT (self->darea_55),
                    "draw",
                    G_CALLBACK (draw_vertical_bar),
                    self);
  g_signal_connect (G_OBJECT (self->darea_12),
                    "draw",
                    G_CALLBACK (draw_conn_cc_ghub),
                    self);
  g_signal_connect (G_OBJECT (self->darea_22),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_gauth),
                    self);
  g_signal_connect (G_OBJECT (self->darea_24),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_gctrl),
                    self);
  g_signal_connect (G_OBJECT (self->darea_14),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_apps),
                    self);
  g_signal_connect (G_OBJECT (self->darea_53),
                    "draw",
                    G_CALLBACK (draw_conn_gagent_gpms),
                    self);
  g_signal_connect (G_OBJECT (self->darea_33),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_gagent),
                    self);
  g_signal_connect (G_OBJECT (self->gpms_button),
                    "button-press-event",
                    G_CALLBACK (gpms_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->gagent_button),
                    "button-press-event",
                    G_CALLBACK (gagent_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->apps_button),
                    "button-press-event",
                    G_CALLBACK (apps_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->gcontroller_button),
                    "button-press-event",
                    G_CALLBACK (gctrl_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (event_box),
                    "button-press-event",
                    G_CALLBACK (log_label_clicked),
                    self);
  //TODO: DELETE_START
  self->cover_label = gtk_label_new ("보안 앱 정책 설정");
  pg_attr_list = pango_attr_list_new ();
  pg_attr = pango_attr_foreground_new (65535, 65535, 65535);
  pango_attr_list_insert (pg_attr_list, pg_attr);
  pg_attr = pango_attr_size_new_absolute (45*PANGO_SCALE);
  pango_attr_list_insert (pg_attr_list, pg_attr);
  gtk_label_set_attributes (GTK_LABEL (self->cover_label), pg_attr_list);
  gtk_widget_set_margin_top (self->cover_label, 130);

  self->cover_sub_label = gtk_label_new ("시나리오를 시작합니다.");
  pg_attr_list = pango_attr_list_new ();
  pg_attr = pango_attr_foreground_new (65535, 65535, 65535);
  pango_attr_list_insert (pg_attr_list, pg_attr);
  pg_attr = pango_attr_size_new_absolute (20*PANGO_SCALE);
  pango_attr_list_insert (pg_attr_list, pg_attr);
  gtk_label_set_attributes (GTK_LABEL (self->cover_sub_label), pg_attr_list);
  gtk_widget_set_margin_top (self->cover_sub_label, 20);

  self->sound_wave = gtk_image_new ();
  gtk_image_set_from_file (GTK_IMAGE (self->sound_wave), SOUND_WAVE_GIF);
  gtk_widget_set_margin_top (self->sound_wave, 70);

  self->cover_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (self->cover_box), self->cover_label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (self->cover_box), self->cover_sub_label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (self->cover_box), self->sound_wave, FALSE, FALSE, 0);

  self->cover_popover = gtk_popover_new (self->lsf_image);
  gtk_widget_set_size_request (self->cover_popover, 700, 500);
  gtk_widget_override_background_color (self->cover_popover, GTK_STATE_FLAG_NORMAL, &cover_color);
  gtk_container_add (GTK_CONTAINER (self->cover_popover), self->cover_box);
  gtk_popover_set_modal (GTK_POPOVER (self->cover_popover), FALSE);

  gtk_widget_show_all (self->module_info_box);
  // DELETE_END

}

GtkWidget *
cc_security_framework_panel_new (void)
{
  return g_object_new (CC_TYPE_SECURITY_FRAMEWORK_PANEL,
                       NULL);
}
