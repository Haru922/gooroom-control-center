/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 *
 * Copyright (C) 2020 gooroom <gooroom@gooroom.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "cc-lsftest-panel.h"
#include "cc-lsftest-resources.h"

#define LSFTEST_SCHEMA "org.gnome.desktop.lsftest"

struct _CcLsftestPanel
{
  CcPanel    parent_instance;

  GtkBuilder *builder;
  GtkWidget *app_book;
  GtkWidget *panel_box[LSF_PANEL_APP_MAX];
  GtkWidget *apply_button[LSF_PANEL_APP_MAX];
  int app_cnt;
  int obj_cnt[LSF_PANEL_APP_MAX];
  char *xml_file[LSF_PANEL_APP_MAX];
  char *tab_name[LSF_PANEL_APP_MAX];

  lsf_app_panel_object *panel_obj[LSF_PANEL_APP_MAX][LSF_PANEL_OBJECT_MAX];
};

G_DEFINE_TYPE (CcLsftestPanel, cc_lsftest_panel, CC_TYPE_PANEL)

static void
cc_lsftest_panel_dispose (GObject *object)
{
  CcLsftestPanel *lsftest_panel = CC_LSFTEST_PANEL (object);

  G_OBJECT_CLASS (cc_lsftest_panel_parent_class)->dispose (object);
}

static void
cc_lsftest_panel_constructed (GObject *object)
{
  CcLsftestPanel *self = CC_LSFTEST_PANEL (object);
}

static void
cc_lsftest_panel_class_init (CcLsftestPanelClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = cc_lsftest_panel_dispose;
  object_class->constructed = cc_lsftest_panel_constructed;
}

static void
apply_button_clicked (GtkWidget *widget,
                      GdkEventButton *event,
                      gpointer user_data)
{
  int i;
  CcLsftestPanel *self = (CcLsftestPanel *) user_data;
  int app_num = gtk_notebook_get_current_page (GTK_NOTEBOOK (self->app_book));
  for (i = 0; i < self->obj_cnt[app_num]; i++)
    g_print ("#%d: %s->%s\n", app_num, self->panel_obj[app_num][i]->object_id, self->panel_obj[app_num][i]->class_name);
}

static void
get_objects (CcLsftestPanel *self, xmlNode *node, int app_num)
{
  xmlNode *cur_node = NULL;

  for (cur_node = node; cur_node; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE &&
        strcmp (cur_node->name, "object") == 0 &&
        xmlHasProp (cur_node, "id"))
    {
      self->panel_obj[app_num][self->obj_cnt[app_num]] = (lsf_app_panel_object *) malloc (sizeof (lsf_app_panel_object));
      self->panel_obj[app_num][self->obj_cnt[app_num]++]->object_id = strdup (xmlGetProp (cur_node, "id"));
    }
    get_objects (self, cur_node->children, app_num);
  }
}

static void
xml_parser (CcLsftestPanel *self, int app_num)
{
  xmlDocPtr doc = xmlReadFile (self->xml_file[app_num], "UTF-8", XML_PARSE_RECOVER);
  xmlNode *root = xmlDocGetRootElement (doc);
  get_objects (self, root, app_num);
  xmlFreeDoc (doc);
}

static void
cc_lsftest_panel_init (CcLsftestPanel *self)
{
  int app_num, obj_num;
  int r;
  lsf_user_data_t app_data;
  g_resources_register (cc_lsftest_get_resource ());

  self->app_book = gtk_notebook_new ();
  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (self->app_book));
  self->app_cnt = 0;
  self->tab_name[self->app_cnt] = "파일 완전 소거";
  self->xml_file[self->app_cnt++] = "/var/tmp/lsf/glade/lsftest.glade";
  //self->tab_name[self->app_cnt] = "gooroom";
  //self->xml_file[self->app_cnt++] = "/var/tmp/lsf/glade/lsftest2.glade";

  for (app_num = 0; app_num < self->app_cnt; app_num++)
  {
    xml_parser (self, app_num);
    self->builder = gtk_builder_new_from_file (self->xml_file[app_num]);
    for (obj_num = 0; obj_num < self->obj_cnt[app_num]; obj_num++)
    {
      self->panel_obj[app_num][obj_num]->object = gtk_builder_get_object (self->builder, self->panel_obj[app_num][obj_num]->object_id);
      self->panel_obj[app_num][obj_num]->class_name = strdup (G_OBJECT_TYPE_NAME (self->panel_obj[app_num][obj_num]->object));
      g_print ("#%d: %s->%s\n", app_num, self->panel_obj[app_num][obj_num]->object_id, self->panel_obj[app_num][obj_num]->class_name);
    }
    self->panel_box[app_num] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    self->apply_button[app_num] = gtk_button_new_with_label ("적용");
    gtk_container_remove (GTK_CONTAINER (self->panel_obj[app_num][0]->object), GTK_WIDGET (self->panel_obj[app_num][1]->object));
    gtk_container_add (GTK_CONTAINER (self->panel_box[app_num]), GTK_WIDGET (self->panel_obj[app_num][1]->object));
    gtk_container_add (GTK_CONTAINER (self->panel_box[app_num]), GTK_WIDGET (self->apply_button[app_num]));
    gtk_notebook_append_page (GTK_NOTEBOOK (self->app_book), self->panel_box[app_num], gtk_label_new (self->tab_name[app_num]));
    g_signal_connect (G_OBJECT (self->apply_button[app_num]),
                      "button-press-event",
                      G_CALLBACK (apply_button_clicked),
                      self);
  }

  r = lsf_auth (&app_data, PASS_PHRASE);
  if (r == LSF_AUTH_STAT_OK)
  {
    lsf_panel_symm_key = g_strdup (app_data.symm_key);
    lsf_panel_access_token = g_strdup (app_data.access_token);
  }

  gtk_widget_show_all (GTK_WIDGET (self->app_book));
}

GtkWidget *
cc_lsftest_panel_new (void)
{
  return g_object_new (CC_TYPE_LSFTEST_PANEL,
                       NULL);
}
