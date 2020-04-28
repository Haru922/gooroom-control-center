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

#include "cc-security-framework-panel.h"
#include "cc-security-framework-resources.h"

#define SECURITY_FRAMEWORK_SCHEMA "org.gnome.desktop.security-framework"

struct _CcSecurityFrameworkPanel
{
  CcPanel parent_instance;

  //GSettings *security_framework_settings;
  GtkWidget *ghub_button;
  GtkWidget *gauth_button;
  GtkWidget *gcontroller_button;
  GtkWidget *apps_button;
  GtkWidget *darea_12;
  GtkWidget *darea_21;
  GtkWidget *darea_23;
  GtkWidget *darea_33;
  GtkWidget *darea_34;
  GtkWidget *info_area;
  GtkWidget *module_name_label;
  GtkWidget *module_text_view;
};

G_DEFINE_TYPE (CcSecurityFrameworkPanel, cc_security_framework_panel, CC_TYPE_PANEL)

static void do_drawing (cairo_t *cr, GtkWidget *, gint type);

static gboolean
on_draw_event (GtkWidget *widget,
               cairo_t *cr,
               gpointer user_data)
{

  gint data = GPOINTER_TO_INT (user_data);
  do_drawing (cr, widget, data);

  return FALSE;
} 

static void
ghub_cell_clicked (GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  gchar *message = "[GHUB]\nSTATE: RUNNING\nGHUB CELL CLICKED\n";
  g_print ("%s", message);
  gtk_label_set_text (GTK_LABEL (self->module_name_label), "GHub");
}

static void
gauth_cell_clicked (GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  gchar *message = "[GAUTH]\nSTATE: RUNNING\nGAUTH CELL CLICKED\n";
  g_print ("%s", message);
  gtk_label_set_text (GTK_LABEL (self->module_name_label), "GAuth");
}

static void
gcontroller_cell_clicked (GtkWidget *widget,
                          GdkEvent  *event,
                          gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  gchar *message = "[GCONTROLLER]\nSTATE: RUNNING\nGCONTROLLER CELL CLICKED\n";
  g_print ("%s", message);
  gtk_label_set_text (GTK_LABEL (self->module_name_label), "GController");
}

static void
apps_cell_clicked (GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   user_data)
{
  CcSecurityFrameworkPanel *self = (CcSecurityFrameworkPanel *) user_data;
  gchar *message = "[APPS]\nSTATE: DEAD\nAPPS CELL CLICKED\n";
  g_print ("%s", message);
  gtk_label_set_text (GTK_LABEL (self->module_name_label), "Apps");
}

static void
cell_click_event (GtkWidget *widget,
                  GdkEvent *event,
                  gpointer user_data)
{
  GtkWidget *dialog, *label, *content_area;
  GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
  gchar *message;
  gint result;
  gint data = GPOINTER_TO_INT (user_data);
  /*
  dialog = gtk_dialog_new_with_buttons ("Security Framework Module",
                                        gtk_widget_get_parent_window (widget),
                                        flags,
                                        "STOP",
                                        GTK_RESPONSE_REJECT,
                                        "LAUNCH",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  */
  switch (data)
  {
    case GHUB_CELL:
      message = "[GHUB]\nSTATE: RUNNING\nGHUB CELL CLICKED\n";
      break;
    case GAUTH_CELL:
      message = "[GAUTH]\nSTATE: RUNNING\nGAUTH CELL CLICKED\n";
      break;
    case GCONTROLLER_CELL:
      message = "[GCONTROLLER]\nSTATE: RUNNING\nGCONTROLLER CELL CLICKED\n";
      break;
    case APPS_CELL:
      message = "[APPS]\nSTATE: RUNNING\nAPPS CELL CLICKED\n";
      break;
  }
  g_print ("%s", message);
  /*
  label = gtk_label_new (message);
  gtk_container_add (GTK_CONTAINER (content_area), label);
  gtk_widget_show_all (dialog);

  result = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (result)
  {
    case GTK_RESPONSE_ACCEPT:
      break;
    case GTK_RESPONSE_REJECT:
      break;
  }
  gtk_widget_destroy (dialog);
  */
}

static void
do_drawing (cairo_t *cr,
            GtkWidget *widget,
            gint flag)
{
  static const double dashed[] = { 5.0 };
  int mid_right[2] = { 10, 35 };
  int mid_left[2] = { 80, 35 };
  int mid_up[2] = { 45, 10 };
  int mid_down[2] = { 45, 60 };
  int mid_mid[2] = { 45, 35 };
  int top_left[2] = { 0, 0 };
  int bottom_right[2] = { 90, 70 };
  int top_right[2] = { 90, 0 };
  int bottom_left[2] = { 0, 70 };

  cairo_set_line_width (cr, 2.0);

  //cairo_set_dash (cr, dashed, 1, 0);
  if (flag & RED)
  {
    cairo_set_source_rgba (cr, 0.71, 0.06, 0.15, 1);
  }
  else if (flag & GREEN)
  {
    cairo_set_source_rgba (cr, 0.44, 0.74, 0.12, 1);
  }
  else if (flag & BLUE)
  {
    cairo_set_source_rgba (cr, 0.05, 0.39, 0.82, 1);
  }
  else
  {
    cairo_set_source_rgba (cr, 0.64, 0.64, 0.64, 1);
  }


  if (flag & VERTICAL)
  {
    cairo_move_to (cr, mid_up[XPOS], mid_up[YPOS]);
    cairo_line_to (cr, mid_down[XPOS], mid_down[YPOS]);
    /*
    if (flag & UP_RIGHT)
    {
      cairo_move_to (cr, 35, 10);
      cairo_line_to (cr, 45, 0);
      cairo_move_to (cr, 55, 10);
      cairo_line_to (cr, 45, 0);
    }
    else if (flag & DOWN_LEFT)
    {
      cairo_move_to (cr, 35, 60);
      cairo_line_to (cr, 45, 70);
      cairo_move_to (cr, 55, 60);
      cairo_line_to (cr, 45, 70);
    }
    */
  }
  else if (flag & HORIZONTAL)
  {
    cairo_move_to (cr, mid_right[XPOS], mid_right[YPOS]);
    cairo_line_to (cr, mid_left[XPOS], mid_left[YPOS]);
    /*
    if (flag & UP_RIGHT)
    {
      cairo_move_to (cr, 80, 25);
      cairo_line_to (cr, 90, 35);
      cairo_move_to (cr, 80, 45);
      cairo_line_to (cr, 90, 35);
    }
    else if (flag & DOWN_LEFT)
    {
      cairo_move_to (cr, 10, 25);
      cairo_line_to (cr, 0, 35);
      cairo_move_to (cr, 10, 45);
      cairo_line_to (cr, 0, 35);
    }
    */
  }
  else if (flag & DIAGONAL)
  {
    if (flag & UP)
    {
      if (flag & RIGHT)
      {
        cairo_move_to (cr, bottom_left[XPOS], bottom_left[YPOS]);
        cairo_line_to (cr, top_right[XPOS], top_right[YPOS]);
      }
      else
      {
        cairo_move_to (cr, bottom_right[XPOS], bottom_right[YPOS]);
        cairo_line_to (cr, top_left[XPOS], top_left[YPOS]);
      }
    }
    else
    {
      if (flag & RIGHT)
      {
        cairo_move_to (cr, top_left[XPOS], top_left[YPOS]);
        cairo_line_to (cr, bottom_right[XPOS], bottom_right[YPOS]);
      }
      else
      {
        cairo_move_to (cr, top_right[XPOS], top_right[YPOS]);
        cairo_line_to (cr, bottom_left[XPOS], bottom_left[YPOS]);
      }
    }
  }

  cairo_stroke (cr);
  gtk_widget_show_all (widget);
}

static gboolean
time_handler (GtkWidget *widget)
{
  
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

  //g_clear_object (&security_framework_panel->security_framework_settings);

  G_OBJECT_CLASS (cc_security_framework_panel_parent_class)->dispose (object);
}

static void
cc_security_framework_panel_constructed (GObject *object)
{
  CcSecurityFrameworkPanel *self = CC_SECURITY_FRAMEWORK_PANEL (object);

  gtk_widget_set_opacity (self->apps_button, 0.3);
  //self->security_framework_settings = g_settings_new (SECURITY_FRAMEWORK_SCHEMA);
  //g_timeout_add (100, (GSourceFunc) time_handler, (gpointer) self);
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
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, apps_button);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_12);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_21);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_23);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_33);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, darea_34);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, module_name_label);
  gtk_widget_class_bind_template_child (widget_class, CcSecurityFrameworkPanel, module_text_view);
}

static void
cc_security_framework_panel_init (CcSecurityFrameworkPanel *self)
{
  g_resources_register (cc_security_framework_get_resource ());

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect (G_OBJECT (self->darea_12),
                    "draw",
                    G_CALLBACK (on_draw_event),
                    GINT_TO_POINTER (HORIZONTAL|GREEN));
  g_signal_connect (G_OBJECT (self->darea_21),
                    "draw",
                    G_CALLBACK (on_draw_event),
                    GINT_TO_POINTER (VERTICAL|GREEN));
  g_signal_connect (G_OBJECT (self->darea_23),
                    "draw",
                    G_CALLBACK (on_draw_event),
                    GINT_TO_POINTER (VERTICAL|GREEN));
  g_signal_connect (G_OBJECT (self->darea_33),
                    "draw",
                    G_CALLBACK (on_draw_event),
                    GINT_TO_POINTER (DIAGONAL|UP|LEFT|RED));
  g_signal_connect (G_OBJECT (self->darea_34),
                    "draw",
                    G_CALLBACK (on_draw_event),
                    GINT_TO_POINTER (HORIZONTAL));
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
                    G_CALLBACK (gcontroller_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->apps_button),
                    "button-press-event",
                    G_CALLBACK (apps_cell_clicked),
                    self);
  g_signal_connect (G_OBJECT (self->darea_34),
                    "button-press-event",
                    G_CALLBACK (cell_click_event),
                    GINT_TO_POINTER (APPS_CELL));
  gtk_text_view_set_editable (GTK_TEXT_VIEW (self->module_text_view), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (self->module_text_view), FALSE);
  gtk_label_set_text (GTK_LABEL (self->module_name_label), "Gooroom Control Center");
}

GtkWidget *
cc_security_framework_panel_new (void)
{
  return g_object_new (CC_TYPE_SECURITY_FRAMEWORK_PANEL,
                       NULL);
}
