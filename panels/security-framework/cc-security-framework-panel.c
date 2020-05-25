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

struct _CcSecurityFrameworkPanel
{
  CcPanel    parent_instance;
  GtkWidget *ghub_button;
  GtkWidget *gauth_button;
  GtkWidget *gcontroller_button;
  GtkWidget *apps_button;
  GtkWidget *gagent_button;
  GtkWidget *darea_12;
  GtkWidget *darea_21;
  GtkWidget *darea_23;
  GtkWidget *darea_33;
  GtkWidget *darea_51;
  GtkWidget *darea_52;
  GtkWidget *darea_32;
  GtkWidget *log_label;
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
  guint      event_source_tag;
  gchar     *log_message[LOG_BUF];
  gint       selected_cell;
  gint       scene;
  gint       scene_cnt;
  gint       direction[LINE_NUM];
  gint       target_cell;
  gint       log_start;
  gint       log_end;
  gint       log_cnt;
};

G_DEFINE_TYPE (CcSecurityFrameworkPanel, cc_security_framework_panel, CC_TYPE_PANEL)

static void do_drawing           (GtkWidget *, cairo_t *, gint, gint, gint, gint);
static void update_module_state  (CcSecurityFrameworkPanel *, gint);
static void menu_item_selected   (GtkWidget *, GdkEvent *, gpointer);
static void set_line_color       (cairo_t *, gint);
static void get_modules_state    (CcSecurityFrameworkPanel *);
static void set_menu_items       (CcSecurityFrameworkPanel *, gint);

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
  do_drawing (widget, cr, self->direction[LINE_CC_GHUB], color, self->scene, self->scene_cnt);
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
  do_drawing (widget, cr, self->direction[LINE_GHUB_GAUTH], color, self->scene, self->scene_cnt);
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
  do_drawing (widget, cr, self->direction[LINE_GHUB_GCTRL], color, self->scene, self->scene_cnt);
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
  do_drawing (widget, cr, self->direction[LINE_GHUB_GAGENT], color, self->scene, self->scene_cnt);
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
  do_drawing (widget, cr, self->direction[LINE_GHUB_APPS], color, self->scene, self->scene_cnt);
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
  do_drawing (widget, cr, self->direction[LINE_GAGENT_GPMS], color, self->scene, self->scene_cnt);
}

static void
update_module_state (CcSecurityFrameworkPanel *self,
                     gint                      module)
{
  if (module == -1)
    return;
  
  if (self->activated[module])
  {
  }
  else
  {
  }
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
ghub_cell_clicked (GtkWidget      *widget,
                   GdkEventButton *event,
                   gpointer        user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  GtkWidget *popover;
  
  self->selected_cell = GHUB_CELL;
  update_module_state (self, GHUB_CELL);

  if (event->button == GDK_BUTTON_PRIMARY)
  {
    gtk_menu_popup_at_pointer (GTK_MENU (self->menu[GHUB_CELL]), NULL);
  }
}

static void
gauth_cell_clicked (GtkWidget      *widget,
                    GdkEventButton *event,
                    gpointer        user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;

  self->selected_cell = GAUTH_CELL;
  update_module_state (self, GAUTH_CELL);

  if (event->button == GDK_BUTTON_PRIMARY)
  {
    gtk_menu_popup_at_pointer (GTK_MENU (self->menu[GAUTH_CELL]), NULL);
  }
}

static void
gctrl_cell_clicked (GtkWidget      *widget,
                    GdkEventButton *event,
                    gpointer        user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  
  self->selected_cell = GCTRL_CELL;
  update_module_state (self, GCTRL_CELL);

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
  
  self->selected_cell = GAGENT_CELL;
  update_module_state (self, GAGENT_CELL);

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

  self->selected_cell = APPS_CELL;

  if (event->button == GDK_BUTTON_PRIMARY)
  {
    gtk_menu_popup_at_pointer (GTK_MENU (self->menu[APPS_CELL]), NULL);
  }
}

static void
default_menu_handler (GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  const gchar *selection = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));

  g_print ("%s\n", selection);
}

static void
gctrl_menu_handler (GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  GtkWidget *app_ctrl_window;
  const gchar *selection = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));

  if (!g_strcmp0 (selection, "Application Control"))
  {
    app_ctrl_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (app_ctrl_window), "Application Control");
    gtk_window_set_default_size (GTK_WINDOW (app_ctrl_window), 400, 400);
    gtk_window_set_position (GTK_WINDOW (app_ctrl_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_show (app_ctrl_window);
  }
  g_print ("%s\n", selection);
}

static void
menu_item_selected (GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  const gchar *selection = gtk_menu_item_get_label (GTK_MENU_ITEM (widget));

  if (!g_strcmp0 (selection, "Received Message"))
  {
  }
  else if (!g_strcmp0 (selection, "Sent Message"))
  {
  }
  else if (!g_strcmp0 (selection, "Application List"))
  {
  }
  g_print ("%s\n", selection);
}

static void
set_line_color (cairo_t *cr,
                gint     color)
{
  switch (color)
  {
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
do_drawing (GtkWidget *widget,
            cairo_t   *cr,
            gint       direction,
            gint       color,
            gint       scene,
            gint       scene_cnt)
{
  int i;
  gboolean scope = FALSE;
  int mid_right[2] = { 75, 35 };
  int mid_left[2] = { 15, 35 };
  int mid_up[2] = { 45, 15 };
  int mid_down[2] = { 45, 60 };
  int mid_mid[2] = { 45, 35 };
  int top_left[2] = { 15, 15 };
  int bottom_right[2] = { 75, 55 };
  int top_right[2] = { 75, 15 };
  int bottom_left[2] = { 0, 70 };

  cairo_set_line_width (cr, 2.0);
  set_line_color (cr, color);

  switch (direction)
  {
    case DIRECTION_DEFAULT_HORIZONTAL:
      for (i = 0; i < 4; i++)
      {
        cairo_arc (cr, mid_left[XPOS]+(HT*i), mid_left[YPOS], RADIUS_MEDIUM, 0, 2*M_PI);
      }
      cairo_fill (cr);
      break;
    case DIRECTION_DEFAULT_VERTICAL:
      for (i = 0; i < 4; i++)
      {
        cairo_arc (cr, mid_up[XPOS], mid_up[YPOS]+(VT*i), RADIUS_MEDIUM, 0, 2*M_PI);
      }
      cairo_fill (cr);
      break;
    case DIRECTION_DEFAULT_DIAGONAL_DOWN_RIGHT:
      for (i = 0; i < 4; i++)
      {
        cairo_arc (cr, top_left[XPOS]+(HT*i), top_left[YPOS]+(VT*i), RADIUS_MEDIUM, 0, 2*M_PI);
      }
      cairo_fill (cr);
      break;
    case DIRECTION_UP:
      switch (scene)
      {
        case SCENE_GHUB_BROADCAST:
          if (SCENE_GHUB_BROADCAST_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GHUB_BROADCAST_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
        case SCENE_APP_REQUEST:
          if (SCENE_APP_REQUEST_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_APP_REQUEST_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
      }
      for (i = 0; i < 4; i++)
      {
        if (scope)
        {
          if (!((scene_cnt+i)%2))
          {
            set_line_color (cr, color);
            cairo_arc (cr, mid_down[XPOS], mid_down[YPOS]-(VT*i), RADIUS_SMALL, 0, 2*M_PI);
          }
          else
          {
            set_line_color (cr, COLOR_YELLOW);
            cairo_arc (cr, mid_down[XPOS], mid_down[YPOS]-(VT*i), RADIUS_LARGE, 0, 2*M_PI);
          }
        }
        else
        {
          set_line_color (cr, color);
          cairo_arc (cr, mid_down[XPOS], mid_down[YPOS]-(VT*i), RADIUS_MEDIUM, 0, 2*M_PI);
        }
        cairo_fill (cr);
      }
      break;
    case DIRECTION_DOWN:
      switch (scene)
      {
        case SCENE_GHUB_BROADCAST:
          if (SCENE_GHUB_BROADCAST_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GHUB_BROADCAST_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
        case SCENE_GCTRL_KILL:
        case SCENE_GCTRL_LAUNCH:
          if (SCENE_GCTRL_KILL_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GCTRL_KILL_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
        case SCENE_GAUTH_RESPONSE:
          if (SCENE_GAUTH_RESPONSE_GAUTH_BLINKING < scene_cnt
              && scene_cnt < SCENE_GAUTH_RESPONSE_GAUTH_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
      }
      for (i = 0; i < 4; i++)
      {
        if (scope)
        {
          if (!((scene_cnt+i)%2))
          {
            set_line_color (cr, color);
            cairo_arc (cr, mid_up[XPOS], mid_up[YPOS]+(VT*i), RADIUS_SMALL, 0, 2*M_PI);
          }
          else
          {
            set_line_color (cr, COLOR_YELLOW);
            cairo_arc (cr, mid_up[XPOS], mid_up[YPOS]+(VT*i), RADIUS_LARGE, 0, 2*M_PI);
          }
        }
        else
        {
          set_line_color (cr, color);
          cairo_arc (cr, mid_up[XPOS], mid_up[YPOS]+(VT*i), RADIUS_MEDIUM, 0, 2*M_PI);
        }
        cairo_fill (cr);
      }
      break;
    case DIRECTION_RIGHT:
      switch (scene)
      {
        case SCENE_GHUB_BROADCAST:
          if (SCENE_GHUB_BROADCAST_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GHUB_BROADCAST_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
        case SCENE_GCTRL_KILL:
        case SCENE_GCTRL_LAUNCH:
          if (SCENE_GCTRL_KILL_CC_BLINKING < scene_cnt
              && scene_cnt < SCENE_GCTRL_KILL_CC_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
        case SCENE_GAUTH_RESPONSE:
          if (SCENE_GAUTH_RESPONSE_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GAUTH_RESPONSE_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
      }
      for (i = 0; i < 4; i++)
      {
        if (scope)
        {
          if ((scene_cnt+i)%2)
          {
            set_line_color (cr, COLOR_YELLOW);
            cairo_arc (cr, mid_left[XPOS]+(HT*i), mid_left[YPOS], RADIUS_LARGE, 0, 2*M_PI);
          }
          else
          {
            set_line_color (cr, color);
            cairo_arc (cr, mid_left[XPOS]+(HT*i), mid_left[YPOS], RADIUS_SMALL, 0, 2*M_PI);
          }
        }
        else
        {
          set_line_color (cr, color);
          cairo_arc (cr, mid_left[XPOS]+(HT*i), mid_left[YPOS], RADIUS_MEDIUM, 0, 2*M_PI);
        }
        cairo_fill (cr);
      }
      break;
    case DIRECTION_LEFT:
      switch (scene)
      {
        case SCENE_POLICY_RELOAD:
          if (SCENE_POLICY_RELOAD_GAGENT_BLINKING < scene_cnt
              && scene_cnt < SCENE_POLICY_RELOAD_GAGENT_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
        case SCENE_GHUB_BROADCAST:
          if (SCENE_GHUB_BROADCAST_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GHUB_BROADCAST_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
      }
      for (i = 0; i < 4; i++)
      {
        if (scope)
        {
          if ((scene_cnt+i)%2)
          {
            set_line_color (cr, COLOR_YELLOW);
            cairo_arc (cr, mid_right[XPOS]-(HT*i), mid_right[YPOS], RADIUS_LARGE, 0, 2*M_PI);
          }
          else
          {
            set_line_color (cr, color);
            cairo_arc (cr, mid_right[XPOS]-(HT*i), mid_right[YPOS], RADIUS_SMALL, 0, 2*M_PI);
          }
        }
        else
        {
          cairo_arc (cr, mid_right[XPOS]-(HT*i), mid_right[YPOS], RADIUS_MEDIUM, 0, 2*M_PI);
        }
        cairo_fill (cr);
      }
      break;
    case DIRECTION_DOWN_RIGHT:
      switch (scene)
      {
        case SCENE_GHUB_BROADCAST:
          if (SCENE_GHUB_BROADCAST_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GHUB_BROADCAST_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
        case SCENE_GAUTH_RESPONSE:
          if (SCENE_GAUTH_RESPONSE_GHUB_BLINKING < scene_cnt
              && scene_cnt < SCENE_GAUTH_RESPONSE_GHUB_BLINKING+6)
          {
            scope = TRUE;
          }
      }
      for (i = 0; i < 4; i++)
      {
        if (scope)
        {
          if ((scene_cnt+i)%2)
          {
            set_line_color (cr, COLOR_YELLOW);
            cairo_arc (cr, top_left[XPOS]+(HT*i), top_left[YPOS]+(VT*i), RADIUS_LARGE, 0, 2*M_PI);
          }
          else
          {
            set_line_color (cr, color);
            cairo_arc (cr, top_left[XPOS]+(HT*i), top_left[YPOS]+(VT*i), RADIUS_SMALL, 0, 2*M_PI);
          }
        }
        else
        {
          set_line_color (cr, color);
          cairo_arc (cr, top_left[XPOS]+(HT*i), top_left[YPOS]+(VT*i), RADIUS_MEDIUM, 0, 2*M_PI);
        }
        cairo_fill (cr);
      }
      break;
    case DIRECTION_UP_LEFT:
      switch (scene)
      {
        case SCENE_APP_REQUEST:
          if (SCENE_APP_REQUEST_APP_BLINKING < scene_cnt
              && scene_cnt < SCENE_APP_REQUEST_APP_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
      }
      for (i = 0; i < 4; i++)
      {
        if (scope)
        {
          if ((scene_cnt+i)%2)
          {
            set_line_color (cr, color);
            cairo_arc (cr, top_left[XPOS]+(HT*i), top_left[YPOS]+(VT*i), RADIUS_SMALL, 0, 2*M_PI);
          }
          else
          {
            set_line_color (cr, COLOR_YELLOW);
            cairo_arc (cr, top_left[XPOS]+(HT*i), top_left[YPOS]+(VT*i), RADIUS_LARGE, 0, 2*M_PI);
          }
        }
        else
        {
          set_line_color (cr, color);
          cairo_arc (cr, top_left[XPOS]+(HT*i), top_left[YPOS]+(VT*i), RADIUS_MEDIUM, 0, 2*M_PI);
        }
        cairo_fill (cr);
      }
      break;
    case DIRECTION_CUSTOM:
      switch (scene)
      {
        case SCENE_POLICY_RELOAD:
          if (SCENE_POLICY_RELOAD_GPMS_BLINKING < scene_cnt
              && scene_cnt < SCENE_POLICY_RELOAD_GPMS_BLINKING+6)
          {
            scope = TRUE;
          }
          break;
      }
      for (i = 0; i < 4; i++)
      {
        if (scope)
        {
          if (!((scene_cnt+i)%2))
          {
            set_line_color (cr, color);
            if (i < 2)
            {
              cairo_arc (cr, mid_up[XPOS], mid_right[YPOS]-(VT*(1-i)), RADIUS_SMALL, 0, 2*M_PI);
            }
            else
            {
              cairo_arc (cr, mid_up[XPOS]-(VT*(i-1)), mid_right[YPOS], RADIUS_SMALL, 0, 2*M_PI);
            }
          }
          else
          {
            set_line_color (cr, COLOR_YELLOW);
            if (i < 2)
            {
              cairo_arc (cr, mid_up[XPOS], mid_right[YPOS]-(VT*(1-i)), RADIUS_LARGE, 0, 2*M_PI);
            }
            else
            {
              cairo_arc (cr, mid_up[XPOS]-(VT*(i-1)), mid_right[YPOS], RADIUS_LARGE, 0, 2*M_PI);
            }
          }
        }
        else
        {
          if (i < 2)
          {
            cairo_arc (cr, mid_up[XPOS], mid_right[YPOS]-(VT*(1-i)), RADIUS_MEDIUM, 0, 2*M_PI);
          }
          else
          {
            cairo_arc (cr, mid_up[XPOS]-(VT*(i-1)), mid_right[YPOS], RADIUS_MEDIUM, 0, 2*M_PI);
          }
        }
        cairo_fill (cr);
      }
      break;
  }
}

static void
get_modules_state (CcSecurityFrameworkPanel *self)
{
  int i;

  for (i = 0; i < CELL_NUM; i++)
  {
    self->activated[i] = TRUE;
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

  if (module == APPS_CELL)
  {
    menu = self->menu[APPS_CELL];
    menu_item = gtk_menu_item_new_with_label ("Application List");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 0, 1);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (menu_item_selected),
                      self);
    gtk_widget_show_all (menu);
  }
  else if (module == GCTRL_CELL)
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
    menu_item = gtk_menu_item_new_with_label ("Application Control");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 1, 2);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (gctrl_menu_handler),
                      self);
    menu_item = gtk_menu_item_new_with_label ("View Log");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 2, 3);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (gctrl_menu_handler),
                      self);
    menu_item = gtk_menu_item_new_with_label ("Kill");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 3, 4);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (gctrl_menu_handler),
                      self);
    gtk_widget_show_all (menu);
  }
  else if (module == GHUB_CELL || module == GAUTH_CELL || module == GAGENT_CELL)
  {
    switch (module)
    {
      case GHUB_CELL:
        menu = self->menu[GHUB_CELL];
        break;
      case GAUTH_CELL:
        menu = self->menu[GAUTH_CELL];
        break;
      case GAGENT_CELL:
        menu = self->menu[GAGENT_CELL];
        break;
    }
    menu_item = gtk_menu_item_new_with_label ("Launch");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 0, 1);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (default_menu_handler),
                      self);
    menu_item = gtk_menu_item_new_with_label ("Kill");
    gtk_menu_attach (GTK_MENU (menu), menu_item, 0, 1, 1, 2);
    g_signal_connect (G_OBJECT (menu_item),
                      "activate",
                      G_CALLBACK (default_menu_handler),
                      self);
    gtk_widget_show_all (menu);
  }
}

static void
set_modules_opacity (CcSecurityFrameworkPanel *self)
{
  if (self->activated[GHUB_CELL])
  {
    gtk_widget_set_opacity (self->ghub_button, 1.0);
  }
  else
  {
    gtk_widget_set_opacity (self->ghub_button, 0.3);
  }

  if (self->activated[GAUTH_CELL])
  {
    gtk_widget_set_opacity (self->gauth_button, 1.0);
  }
  else
  {
    gtk_widget_set_opacity (self->gauth_button, 0.3);
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
  gtk_widget_queue_draw (self->darea_21);
  gtk_widget_queue_draw (self->darea_23);
  gtk_widget_queue_draw (self->darea_33);
  gtk_widget_queue_draw (self->darea_32);
  gtk_widget_queue_draw (self->darea_52);
}

static void
set_scene_conf (CcSecurityFrameworkPanel *self)
{
  int i;

  switch (self->scene)
  {
    case SCENE_IDLE:
      self->direction[LINE_CC_GHUB] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_GAUTH] = DIRECTION_DEFAULT_VERTICAL;
      self->direction[LINE_GHUB_GCTRL] = DIRECTION_DEFAULT_VERTICAL;
      self->direction[LINE_GHUB_GAGENT] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_APPS] = DIRECTION_DEFAULT_DIAGONAL_DOWN_RIGHT;
      self->direction[LINE_GAGENT_GPMS] = DIRECTION_CUSTOM;
      break;
    case SCENE_GHUB_BROADCAST:
      self->direction[LINE_CC_GHUB] = DIRECTION_LEFT;
      self->direction[LINE_GHUB_GAUTH] = DIRECTION_UP;
      self->direction[LINE_GHUB_GCTRL] = DIRECTION_DOWN;
      self->direction[LINE_GHUB_GAGENT] = DIRECTION_RIGHT;
      self->direction[LINE_GHUB_APPS] = DIRECTION_DOWN_RIGHT;
      self->direction[LINE_GAGENT_GPMS] = DIRECTION_CUSTOM;
      break;
    case SCENE_POLICY_RELOAD:
      self->direction[LINE_CC_GHUB] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_GAUTH] = DIRECTION_DEFAULT_VERTICAL;
      self->direction[LINE_GHUB_GCTRL] = DIRECTION_DEFAULT_VERTICAL;
      self->direction[LINE_GHUB_GAGENT] = DIRECTION_LEFT;
      self->direction[LINE_GHUB_APPS] = DIRECTION_DEFAULT_DIAGONAL_DOWN_RIGHT;
      self->direction[LINE_GAGENT_GPMS] = DIRECTION_CUSTOM;
      break; 
    case SCENE_GCTRL_KILL:
    case SCENE_GCTRL_LAUNCH:
      self->direction[LINE_CC_GHUB] = DIRECTION_RIGHT;
      self->direction[LINE_GHUB_GAUTH] = DIRECTION_DEFAULT_VERTICAL;
      self->direction[LINE_GHUB_GCTRL] = DIRECTION_DOWN;
      self->direction[LINE_GHUB_GAGENT] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_APPS] = DIRECTION_DEFAULT_DIAGONAL_DOWN_RIGHT;
      self->direction[LINE_GAGENT_GPMS] = DIRECTION_CUSTOM;
      break;
    case SCENE_APP_REQUEST:
      self->direction[LINE_CC_GHUB] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_GAUTH] = DIRECTION_UP;
      self->direction[LINE_GHUB_GCTRL] = DIRECTION_DEFAULT_VERTICAL;
      self->direction[LINE_GHUB_GAGENT] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_APPS] = DIRECTION_UP_LEFT;
      self->direction[LINE_GAGENT_GPMS] = DIRECTION_CUSTOM;
      break;
    case SCENE_GAUTH_RESPONSE:
      self->direction[LINE_CC_GHUB] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_GAUTH] = DIRECTION_DOWN;
      self->direction[LINE_GHUB_GCTRL] = DIRECTION_DEFAULT_VERTICAL;
      self->direction[LINE_GHUB_GAGENT] = DIRECTION_DEFAULT_HORIZONTAL;
      self->direction[LINE_GHUB_APPS] = DIRECTION_DOWN_RIGHT;
      self->direction[LINE_GAGENT_GPMS] = DIRECTION_CUSTOM;
      break;
  }
}

static void
scene_handler (CcSecurityFrameworkPanel *self)
{
  set_scene_conf (self);

  if (self->scene == SCENE_IDLE)
  {
    self->animating = FALSE;
    draw_lines (self);
    self->scene_cnt = 0;
  }
  else
  {
    switch (self->scene_cnt)
    {
      case 0:
        switch (self->scene)
        {
          case SCENE_PRESENTING:
            gtk_widget_set_opacity (self->cover_popover, 0.75);
            gtk_image_set_from_file (GTK_IMAGE (self->lsf_image), LSF_IMG);
            gtk_widget_show_all (self->cover_popover);
            gtk_image_set_from_file (GTK_IMAGE (self->sound_wave), "images/sound-wave.gif");
            g_spawn_command_line_async ("/usr/bin/aplay /home/haru/policy-reload.wav", NULL);
            break;
          case SCENE_POLICY_RELOAD:
            gtk_label_set_text (GTK_LABEL (self->message_label[GPMS_CELL]),
                                "<b><span font='12' font-weight='bold'  background='#ffffff' foreground='#6495ed'>보안 정책 변경</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[GPMS_CELL]), TRUE);
            enqueue_log_label (self, "GPMS: 보안 정책 변경");
            renew_log_label (self);
            gtk_popover_popup (GTK_POPOVER (self->popover[GPMS_CELL]));
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_label_set_text (GTK_LABEL (self->message_label[GHUB_CELL]),
                                "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 전달</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[GHUB_CELL]), TRUE);
            enqueue_log_label (self, "GHUB: 보안 정책 변경 전달");
            renew_log_label (self);
            gtk_popover_popup (GTK_POPOVER (self->popover[GHUB_CELL]));
            break;
          case SCENE_GCTRL_KILL:
            gtk_label_set_text (GTK_LABEL (self->message_label[CC_CELL]),
                                "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 정지 요청</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[CC_CELL]), TRUE);
            enqueue_log_label (self, "CC: 앱 정지 요청");
            renew_log_label (self);
            gtk_popover_popup (GTK_POPOVER (self->popover[CC_CELL]));
            break;
          case SCENE_GCTRL_LAUNCH:
            gtk_label_set_text (GTK_LABEL (self->message_label[CC_CELL]),
                                "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 실행 요청</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[CC_CELL]), TRUE);
            enqueue_log_label (self, "CC: 앱 실행 요청");
            renew_log_label (self);
            gtk_popover_popup (GTK_POPOVER (self->popover[CC_CELL]));
            break;
          case SCENE_APP_REQUEST:
            gtk_label_set_text (GTK_LABEL (self->message_label[APPS_CELL]),
                                "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 인증 요청</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[APPS_CELL]), TRUE);
            enqueue_log_label (self, "APPS: 앱 인증 요청");
            renew_log_label (self);
            gtk_popover_popup (GTK_POPOVER (self->popover[APPS_CELL]));
            break;
          case SCENE_GAUTH_RESPONSE:
            gtk_label_set_text (GTK_LABEL (self->message_label[GAUTH_CELL]),
                                "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 인증 확인</span></b>");
            gtk_label_set_use_markup (GTK_LABEL (self->message_label[GAUTH_CELL]), TRUE);
            enqueue_log_label (self, "GAUTH: 앱 인증 확인");
            renew_log_label (self);
            gtk_popover_popup (GTK_POPOVER (self->popover[GAUTH_CELL]));
            break;
        }
        break;
      case 1: case 3: case 5:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG);
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG);
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
            gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG);
            break;
          case SCENE_APP_REQUEST:
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG);
            break;
          case SCENE_GAUTH_RESPONSE:
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG);
            break;
        }
        break;
      case 2: case 4:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            gtk_image_set_from_file (GTK_IMAGE (self->gpms_image), GPMS_IMG_SMALL);
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG_SMALL);
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
            gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG_SMALL);
            break;
          case SCENE_APP_REQUEST:
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG_SMALL);
            break;
          case SCENE_GAUTH_RESPONSE:
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG_SMALL);
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
            gtk_widget_queue_draw (self->darea_52);
            break;
          case SCENE_GHUB_BROADCAST:
            if (self->scene_cnt == 6)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GHUB_CELL]));
            }
            gtk_widget_queue_draw (self->darea_12);
            gtk_widget_queue_draw (self->darea_21);
            gtk_widget_queue_draw (self->darea_23);
            gtk_widget_queue_draw (self->darea_32);
            gtk_widget_queue_draw (self->darea_33);
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
            if (self->scene_cnt == 6)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[CC_CELL]));
            }
            gtk_widget_queue_draw (self->darea_12);
            break;
          case SCENE_APP_REQUEST:
            if (self->scene_cnt == 6)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[APPS_CELL]));
            }
            gtk_widget_queue_draw (self->darea_33);
            break;
          case SCENE_GAUTH_RESPONSE:
            if (self->scene_cnt == 6)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GAUTH_CELL]));
            }
            gtk_widget_queue_draw (self->darea_21);
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
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 변경 전달</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GAGENT_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[GAGENT_CELL]));
              enqueue_log_label (self, "GAGENT: 보안 정책 변경 전달");
              renew_log_label (self);
            }
            gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG);
            break;
          case SCENE_GHUB_BROADCAST:
            if (self->scene_cnt == 11)
            {
              gtk_label_set_text (GTK_LABEL (self->message_label[CC_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 반영</span></b>");
              gtk_label_set_text (GTK_LABEL (self->message_label[GAUTH_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 반영</span></b>");
              gtk_label_set_text (GTK_LABEL (self->message_label[GCTRL_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 반영</span></b>");
              gtk_label_set_text (GTK_LABEL (self->message_label[GAGENT_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 반영</span></b>");
              gtk_label_set_text (GTK_LABEL (self->message_label[APPS_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 반영</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[CC_CELL]), TRUE);
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GAUTH_CELL]), TRUE);
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GCTRL_CELL]), TRUE);
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GAGENT_CELL]), TRUE);
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[APPS_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[CC_CELL]));
              gtk_popover_popup (GTK_POPOVER (self->popover[GAUTH_CELL]));
              gtk_popover_popup (GTK_POPOVER (self->popover[GCTRL_CELL]));
              gtk_popover_popup (GTK_POPOVER (self->popover[GAGENT_CELL]));
              gtk_popover_popup (GTK_POPOVER (self->popover[APPS_CELL]));
              enqueue_log_label (self, "CC: 보안 정책 반영");
              enqueue_log_label (self, "GAUTH: 보안 정책 반영");
              enqueue_log_label (self, "GCTRL: 보안 정책 반영");
              enqueue_log_label (self, "GAGENT: 보안 정책 반영");
              enqueue_log_label (self, "APPS: 보안 정책 반영");
              renew_log_label (self);
            }
            gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG);
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG);
            gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG);
            gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG);
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG);
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
          case SCENE_APP_REQUEST:
          case SCENE_GAUTH_RESPONSE:
            if (self->scene_cnt == 11)
            {
              gtk_label_set_text (GTK_LABEL (self->message_label[GHUB_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>메시지 전달</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GHUB_CELL]), TRUE);
              enqueue_log_label (self, "GHUB: 메시지 전달");
              renew_log_label (self);
              gtk_popover_popup (GTK_POPOVER (self->popover[GHUB_CELL]));
            }
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG);
            break;
        }
        break;
      case 12: case 14:
        switch (self->scene)
        {
          case SCENE_PRESENTING:
            if (self->scene_cnt == 14)
            {
              gtk_widget_hide (self->cover_popover);
              gtk_image_clear (self->lsf_image);
              self->scene = SCENE_POLICY_RELOAD;
              self->scene_cnt = SCENE_END;
            }
            break;
          case SCENE_POLICY_RELOAD:
            gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG_SMALL);
            break;
          case SCENE_GHUB_BROADCAST:
            gtk_image_set_from_file (GTK_IMAGE (self->control_center_image), CC_IMG_SMALL);
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG_SMALL);
            gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG_SMALL);
            gtk_image_set_from_file (GTK_IMAGE (self->gagent_image), GAGENT_IMG_SMALL);
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG_SMALL);
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
          case SCENE_APP_REQUEST:
          case SCENE_GAUTH_RESPONSE:
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG_SMALL);
            break;
        }
        break;
      case 16: case 17: case 18: case 19: case 20: 
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            if (self->scene_cnt == 16)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GAGENT_CELL]));
            }
            gtk_widget_queue_draw (self->darea_32);
            break;
          case SCENE_GHUB_BROADCAST:
            if (self->scene_cnt == 16)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[CC_CELL]));
              gtk_popover_popdown (GTK_POPOVER (self->popover[GAUTH_CELL]));
              gtk_popover_popdown (GTK_POPOVER (self->popover[GCTRL_CELL]));
              gtk_popover_popdown (GTK_POPOVER (self->popover[GAGENT_CELL]));
              gtk_popover_popdown (GTK_POPOVER (self->popover[APPS_CELL]));
              self->scene = SCENE_GCTRL_KILL;
              self->scene_cnt = SCENE_END;
            }
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
            if (self->scene_cnt == 16)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GHUB_CELL]));
            }
            gtk_widget_queue_draw (self->darea_23);
            break;
          case SCENE_APP_REQUEST:
            if (self->scene_cnt == 16)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GHUB_CELL]));
            }
            gtk_widget_queue_draw (self->darea_21);
            break;
          case SCENE_GAUTH_RESPONSE:
            if (self->scene_cnt == 16)
            {
              gtk_popover_popdown (GTK_POPOVER (self->popover[GHUB_CELL]));
            }
            gtk_widget_queue_draw (self->darea_33);
            break;
        }
        break;
      case 21: case 23: case 25:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            if (self->scene_cnt == 21)
            {
              gtk_label_set_text (GTK_LABEL (self->message_label[GHUB_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>보안 정책 반영</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GHUB_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[GHUB_CELL]));
              enqueue_log_label (self, "GHUB: 보안 정책 반영");
              renew_log_label (self);
            }
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG);
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
            if (self->scene_cnt == 21)
            {
              if (self->scene == SCENE_GCTRL_KILL)
              {
                gtk_label_set_text (GTK_LABEL (self->message_label[GCTRL_CELL]),
                                    "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 정지</span></b>");
                gtk_label_set_use_markup (GTK_LABEL (self->message_label[GCTRL_CELL]), TRUE);
                enqueue_log_label (self, "GCTRL: 앱 정지");
              }
              else
              {
                gtk_label_set_text (GTK_LABEL (self->message_label[GCTRL_CELL]),
                                    "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 실행</span></b>");
                gtk_label_set_use_markup (GTK_LABEL (self->message_label[GCTRL_CELL]), TRUE);
                enqueue_log_label (self, "GCTRL: 앱 실행");
              }
              gtk_popover_popup (GTK_POPOVER (self->popover[GCTRL_CELL]));
              renew_log_label (self);
            }
            gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG);
            break;
          case SCENE_APP_REQUEST:
            if (self->scene_cnt == 21)
            {
              gtk_label_set_text (GTK_LABEL (self->message_label[GAUTH_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 인증 요청 확인</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[GAUTH_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[GAUTH_CELL]));
              enqueue_log_label (self, "GAUTH: 앱 인증 요청 확인");
              renew_log_label (self);
            }
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG);
            break;
          case SCENE_GAUTH_RESPONSE:
            if (self->scene_cnt == 21)
            {
              gtk_label_set_text (GTK_LABEL (self->message_label[APPS_CELL]),
                                  "<b><span font='12' font-weight='bold' background='#ffffff' foreground='#6495ed'>앱 인증 결과 확인</span></b>");
              gtk_label_set_use_markup (GTK_LABEL (self->message_label[APPS_CELL]), TRUE);
              gtk_popover_popup (GTK_POPOVER (self->popover[APPS_CELL]));
              enqueue_log_label (self, "APPS: 앱 인증 결과 확인");
              renew_log_label (self);
            }
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG);
            break;
        }
        break;
      case 22: case 24:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            gtk_image_set_from_file (GTK_IMAGE (self->ghub_image), GHUB_IMG_SMALL);
            break;
          case SCENE_GCTRL_KILL:
          case SCENE_GCTRL_LAUNCH:
            gtk_image_set_from_file (GTK_IMAGE (self->gcontroller_image), GCTRL_IMG_SMALL);
            break;
          case SCENE_APP_REQUEST:
            gtk_image_set_from_file (GTK_IMAGE (self->gauth_image), GAUTH_IMG_SMALL);
            break;
          case SCENE_GAUTH_RESPONSE:
            gtk_image_set_from_file (GTK_IMAGE (self->apps_image), APPS_IMG_SMALL);
            break;
        }
        break;
      case 26:
        switch (self->scene)
        {
          case SCENE_POLICY_RELOAD:
            gtk_popover_popdown (GTK_POPOVER (self->popover[GHUB_CELL]));
            self->scene = SCENE_GHUB_BROADCAST;
            break;
          case SCENE_GCTRL_KILL:
            gtk_popover_popdown (GTK_POPOVER (self->popover[GCTRL_CELL]));
            self->activated[self->target_cell] = FALSE; // TODO: Kill Application
            self->scene = SCENE_GCTRL_LAUNCH;
            break;
          case SCENE_GCTRL_LAUNCH:
            gtk_popover_popdown (GTK_POPOVER (self->popover[GCTRL_CELL]));
            self->activated[self->target_cell] = TRUE; // TODO: Launch Application
            self->scene = SCENE_APP_REQUEST;
            break;
          case SCENE_APP_REQUEST:
            gtk_popover_popdown (GTK_POPOVER (self->popover[GAUTH_CELL]));
            self->scene = SCENE_GAUTH_RESPONSE;
            break;
          case SCENE_GAUTH_RESPONSE:
            gtk_popover_popdown (GTK_POPOVER (self->popover[APPS_CELL]));
            self->scene = SCENE_IDLE;
            break;
        }
        break;
    }
    self->scene_cnt = (self->scene_cnt+1)%SCENE_CNT;
  }
}

static void
get_scene (CcSecurityFrameworkPanel *self)
{
  if (self->scene == SCENE_IDLE) 
  {
    self->scene = SCENE_PRESENTING;
  }
}

static gboolean
time_handler (GObject *object)
{
  CcSecurityFrameworkPanel *self = CC_SECURITY_FRAMEWORK_PANEL (object);

  get_scene (self);
  scene_handler (self);
  //update_module_state (self, self->selected_cell);
  if (!self->animating)
  {
    set_modules_opacity (self);
  }
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

  g_source_remove (security_framework_panel->event_source_tag);

  G_OBJECT_CLASS (cc_security_framework_panel_parent_class)->dispose (object);
}

static void
cc_security_framework_panel_constructed (GObject *object)
{
  CcSecurityFrameworkPanel *self = CC_SECURITY_FRAMEWORK_PANEL (object);
  self->event_source_tag = g_timeout_add (250, (GSourceFunc) time_handler, (gpointer) object);
}

static void
cc_security_framework_panel_class_init (CcSecurityFrameworkPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = cc_security_framework_panel_dispose;
  object_class->constructed = cc_security_framework_panel_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/security-framework/security-framework.ui");
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, ghub_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gauth_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gcontroller_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, gagent_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, apps_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_12);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_21);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_23);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_33);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_32);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_51);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_52);
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

  g_resources_register (cc_security_framework_get_resource ());

  gtk_widget_init_template (GTK_WIDGET (self));
  gdk_rgba_parse (&cover_color, "#000000");

  get_modules_state (self);
  set_modules_opacity (self); 

  self->log_start = 0;
  self->log_end = -1;
  self->log_cnt = 0;

  self->selected_cell = -1;
  self->scene = SCENE_IDLE;
  self->target_cell = APPS_CELL;
  set_scene_conf (self);

  event_box = gtk_event_box_new ();
  gtk_event_box_set_above_child (GTK_EVENT_BOX (event_box), FALSE);

  self->popover[CC_CELL] = gtk_popover_new (self->control_center_image);
  self->popover[GHUB_CELL] = gtk_popover_new (self->ghub_image);
  self->popover[GAUTH_CELL] = gtk_popover_new (self->gauth_image);
  self->popover[GCTRL_CELL] = gtk_popover_new (self->gcontroller_image);
  self->popover[GAGENT_CELL] = gtk_popover_new (self->gagent_image);
  self->popover[APPS_CELL] = gtk_popover_new (self->apps_image);
  self->popover[GPMS_CELL] = gtk_popover_new (self->gpms_image);

  for (module = 0; module < CELL_NUM; module++)
  {
    self->menu[module] = gtk_menu_new ();
    set_menu_items (self, module);

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
  gtk_widget_set_size_request (self->cover_popover, 600, 450);
  gtk_widget_override_background_color (self->cover_popover, GTK_STATE_FLAG_NORMAL, &cover_color);
  gtk_container_add (GTK_CONTAINER (self->cover_popover), self->cover_box);
  gtk_popover_set_modal (GTK_POPOVER (self->cover_popover), FALSE);

  gtk_widget_show_all (self->module_info_box);

  g_signal_connect (G_OBJECT (self->darea_12),
                    "draw",
                    G_CALLBACK (draw_conn_cc_ghub),
                    self);
  g_signal_connect (G_OBJECT (self->darea_21),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_gauth),
                    self);
  g_signal_connect (G_OBJECT (self->darea_23),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_gctrl),
                    self);
  g_signal_connect (G_OBJECT (self->darea_33),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_apps),
                    self);
  g_signal_connect (G_OBJECT (self->darea_52),
                    "draw",
                    G_CALLBACK (draw_conn_gagent_gpms),
                    self);
  g_signal_connect (G_OBJECT (self->darea_32),
                    "draw",
                    G_CALLBACK (draw_conn_ghub_gagent),
                    self);
  g_signal_connect (G_OBJECT (self->ghub_button),
                    "button-press-event",
                    G_CALLBACK (ghub_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->gauth_button),
                    "button-press-event",
                    G_CALLBACK (gauth_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->gcontroller_button),
                    "button-press-event",
                    G_CALLBACK (gctrl_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->gagent_button),
                    "button-press-event",
                    G_CALLBACK (gagent_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->apps_button),
                    "button-press-event",
                    G_CALLBACK (apps_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (event_box),
                    "button-press-event",
                    G_CALLBACK (log_label_clicked),
                    self);
}

GtkWidget *
cc_security_framework_panel_new (void)
{
  return g_object_new (CC_TYPE_SECURITY_FRAMEWORK_PANEL,
                       NULL);
}
