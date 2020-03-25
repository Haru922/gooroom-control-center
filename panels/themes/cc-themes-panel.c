/*
 * Copyright (C) 2010 Intel, Inc
 * Copyright (C) 2019 gooroom <gooroom@gooroom.kr>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "cc-themes-panel.h"
//#include "font-dialog.h"
#include "cc-themes-resources.h"

#include <config.h>
#include <gtk/gtk.h>

#include <glib/gi18n.h>

#define DEFAULT_GTK_THEME   "Default"
#define DEFAULT_WM_THEME    "Default"
#define DEFAULT_ICON_THEME  "Rodent"

//#define DEFAULT_THEME_DIR   "/usr/share/themes"
#define DEFAULT_THEME_DIR   "/usr/share/icons"

#define WID(y) (GtkWidget *) gtk_builder_get_object (panel->builder, y)

struct {
  char *icon;
  char *icon_name;
  char *bg_name;
} icon_themes[] = {
  { "Gooroom-Arc", N_("Gooroom Icon Theme 1"), "file:///usr/share/backgrounds/gooroom/gooroom_theme_bg_1.jpg" },
  { "Gooroom-Faenza", N_("Gooroom Icon Theme 2"), "file:///usr/share/backgrounds/gooroom/gooroom_theme_bg_2.jpg" },
  { "Gooroom-Papirus", N_("Gooroom Icon Theme 3"), "file:///usr/share/backgrounds/gooroom/gooroom_theme_bg_3.jpg" },
};

enum
{
  COL_THUMBNAIL,
  COL_ICON_THEME,
  COL_ICON_NAME,
  COL_THUMB_PATH,
  COL_BACKGROUND_PATH,
  NUM_COLS
};

struct _CcThemesPanel
{
  CcPanel           parent_instance;

  GtkBuilder        *builder;

  GSettings         *interface_settings;
  GSettings         *wm_settings;
  GSettings         *bg_settings;

  GtkIconView       *icon_view;
  GtkListStore      *themes_liststore;
  GtkDrawingArea    *current_theme_drawingarea;
  GtkWidget         *theme_apply_button;
  GtkWidget         *themes_list_sw;

  GdkPixbuf         *current_theme_thumbnail;

  gchar             *selected_icon_name;
  gchar             *selected_bg_name;
  gchar             *current_icon_name;
  gchar             *current_bg_name;
};

CC_PANEL_REGISTER (CcThemesPanel, cc_themes_panel)

static const char *
cc_themes_panel_get_help_uri (CcPanel *panel)
{
  return "help:gnome-help/themes";
}

static void
set_thumbnail (CcThemesPanel *panel, gchar *thumb_path)
{
  GdkPixbuf *old_pixbuf;
  old_pixbuf = panel->current_theme_thumbnail;
  gint width;
  gint height;

  if (panel->current_theme_thumbnail != NULL)
  {
    //g_object_unref (panel->current_theme_thumbnail);
    g_clear_object (&panel->current_theme_thumbnail);
    panel->current_theme_thumbnail = NULL;
  }

  panel->current_theme_thumbnail = gdk_pixbuf_new_from_file (thumb_path, NULL);

// FIXME: redraw drawingarea
  gtk_widget_get_size_request (WID ("current-theme-drawingarea"), &width, &height);
  gtk_widget_queue_draw_area (WID ("current-theme-drawingarea"), 0, 0, width, height);

}

static gboolean
on_draw_theme (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  GdkPixbuf *pixbuf;
  CcThemesPanel *panel = CC_THEMES_PANEL (data);
  cairo_t *_cr;
  GdkRGBA color;

  color.red = 1.0;
  color.green = 1.0;
  color.blue = 1.0;
  color.alpha = 1.0;

  pixbuf = gdk_pixbuf_scale_simple (panel->current_theme_thumbnail, 302, 181, GDK_INTERP_BILINEAR);
  _cr = gdk_cairo_create (gtk_widget_get_window (widget));

  // border color
  gdk_cairo_set_source_rgba (_cr, &color);

  gdk_cairo_set_source_pixbuf (_cr, pixbuf, 20, 22);
  cairo_paint (_cr);
  cairo_destroy (_cr);

  return FALSE;
}

static gint
compare_func (gconstpointer a, gconstpointer b)
{
  return g_strcmp0 ((gchar*)a, (gchar*)b);
}

static gint
sort_func (GtkTreeModel *model,
           GtkTreeIter  *a,
           GtkTreeIter  *b,
           gpointer      user_data)
{
  char *theme_a, *theme_b;
  int ret;
  gtk_tree_model_get (model, a,
                      COL_ICON_THEME, &theme_a, -1);

  gtk_tree_model_get (model, b,
                      COL_ICON_THEME, &theme_b, -1);

  ret = g_utf8_collate (theme_a, theme_b);

  g_free (theme_a);
  g_free (theme_b);
}

static GtkListStore*
create_store (void)
{
  GtkListStore *store;

  store = gtk_list_store_new (NUM_COLS,
                              GDK_TYPE_PIXBUF,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING);

  gtk_tree_sortable_set_default_sort_func (GTK_TREE_SORTABLE (store),
                                           sort_func, NULL, NULL);

  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (store),
                                        GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
                                        GTK_SORT_ASCENDING);

  return store;
}

static GSList*
sorted_theme_dirs_get ()
{
  GSList *list = NULL;
  GDir *dir;
  const gchar *file;

  dir = g_dir_open (DEFAULT_THEME_DIR, 0, NULL);

  if (G_UNLIKELY (dir == NULL))
    return NULL;

  /* Iterate over filenames in the directory */
  while ((file = g_dir_read_name (dir)) != NULL) {
      list = g_slist_append (list, g_strdup (file));
  }

  g_dir_close (dir);

  list = g_slist_sort (list, compare_func);

  return list;
}

static void
set_current_theme_label (CcThemesPanel *panel, gchar *icon_name)
{
  for (int i = 0; i < G_N_ELEMENTS (icon_themes); i++)
  {
    if (g_strcmp0 (icon_themes[i].icon, icon_name) == 0)
    {
      gchar **name = g_strsplit (icon_themes[i].icon_name, " ", -1);
      gchar *theme_name = g_strdup_printf (_("Gooroom Icon Theme %s"), name[3]);

      gtk_label_set_text (WID ("current-theme-label"), theme_name);

      g_strfreev (name);
      g_free (theme_name);
    }
  }
}

static GtkListStore *
load_themes_liststore (CcThemesPanel *panel)
{
  GSList *theme_dirs = sorted_theme_dirs_get ();
  GFile *theme_f;
  GFileInputStream *theme_fs;
  GDataInputStream *theme_ds;
  GCancellable *cancellable;
  g_autoptr(GError) err = NULL;
  g_autoptr(GError) inner_err = NULL;
  gchar *line;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkTreeIter pre_iter;
  GtkWidget *current_theme_drawingarea;
  g_autofree char *current_theme = NULL;

  if (!theme_dirs)
    return FALSE;

  GSList *l = NULL;
  GSList *check_list = NULL;

  // add screen data in list store
  store = create_store ();
  for (l = theme_dirs; l; l = l->next)
  {
    g_autofree gchar *theme_dir = NULL;
    g_autofree gchar *thumb_path = NULL;
    g_autofree gchar *icon_name = NULL;
    gchar *background_dir = NULL;
    gchar *gtkrc_filename = g_build_filename (DEFAULT_THEME_DIR, theme_dir, NULL);
    gboolean is_gooroom_theme = FALSE;

    theme_dir = g_strdup ((gchar *) l->data);

    for (int i = 0; i < G_N_ELEMENTS (icon_themes); i++)
    {
      if (g_strcmp0 (icon_themes[i].icon, theme_dir) == 0)
      {
        gchar **name = g_strsplit (icon_themes[i].icon_name, " ", -1);
        icon_name = g_strdup_printf (_("Gooroom Icon Theme %s"), name[3]);

        is_gooroom_theme = TRUE;

        // get background directory from struct list
        background_dir = icon_themes[i].bg_name;

        g_strfreev (name);
        break;
      }
    }

    if (!is_gooroom_theme)
      continue;

    thumb_path = g_strdup_printf ("%s/%s/thumbnail.png", DEFAULT_THEME_DIR, theme_dir);

    if (g_file_test (gtkrc_filename, G_FILE_TEST_EXISTS)
      && g_slist_find_custom (check_list, theme_dir, (GCompareFunc) g_utf8_collate) == NULL)
    {
      current_theme = g_settings_get_string (panel->interface_settings, "icon-theme");
      if (g_strcmp0 (current_theme, theme_dir) == 0)
      {
        set_thumbnail (panel, thumb_path);
        set_current_theme_label (panel, theme_dir);

        //panel->current_icon_name = theme_dir;
        //panel->current_bg_name = background_dir;
      }

      check_list = g_slist_prepend (check_list, g_strdup (theme_dir));

      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
                          COL_THUMBNAIL, gdk_pixbuf_new_from_file_at_size (thumb_path, 180, 110, NULL),
                          COL_ICON_THEME, theme_dir ? g_strdup (theme_dir) : NULL,
                          COL_ICON_NAME, icon_name ? g_strdup (_(icon_name)) : NULL,
                          COL_BACKGROUND_PATH, background_dir ? g_strdup (background_dir) : NULL,
                          COL_THUMB_PATH, thumb_path, -1);
    }
  }

  if (G_LIKELY (theme_dirs)) {
    g_slist_foreach (theme_dirs, (GFunc)g_free, NULL);
    g_slist_free (theme_dirs);
  }

  if (G_LIKELY (check_list)) {
    g_slist_foreach (check_list, (GFunc) g_free, NULL);
    g_slist_free (check_list);
  }

  return store;
}

static void 
on_selection_theme (GtkIconView *icon_view,
                    CcThemesPanel *panel)
{
  GList *list;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *icon_name;
  gchar *background_dir;
  gchar *thumb_path;
  GtkWidget *thumbnail;

  list = gtk_icon_view_get_selected_items (icon_view);
  if (list == NULL)
    return;

  model = gtk_icon_view_get_model (GTK_ICON_VIEW (icon_view));

  if (gtk_tree_model_get_iter (model, &iter, (GtkTreePath*)list->data) == FALSE)
    goto bail;

  gtk_tree_model_get (model, &iter, COL_THUMBNAIL, &thumbnail,
                      COL_ICON_THEME, &panel->selected_icon_name,
                      COL_BACKGROUND_PATH, &panel->selected_bg_name,
                      COL_THUMB_PATH, &thumb_path, -1);

  set_thumbnail (panel, thumb_path);

  g_settings_set_string (panel->interface_settings, "icon-theme", panel->selected_icon_name);
  g_settings_set_string (panel->bg_settings, "picture-uri", panel->selected_bg_name);

bail:
  g_list_free_full (list, (GDestroyNotify) gtk_tree_path_free);

}

static gboolean
on_clicked_apply (GtkButton *button, CcThemesPanel *panel)
{
  GList *list;
  GtkTreeModel *model;
  GtkTreeIter iter;

  list = gtk_icon_view_get_selected_items (panel->icon_view);
  if (list == NULL)
    return FALSE;
  model = gtk_icon_view_get_model (GTK_ICON_VIEW (panel->icon_view));

  if (gtk_tree_model_get_iter (model, &iter, (GtkTreePath*)list->data) == FALSE)
    goto bail;

  panel->current_icon_name = panel->selected_icon_name;
  panel->current_bg_name =  panel->selected_bg_name;

  set_current_theme_label (panel, panel->current_icon_name);

  g_settings_set_string (panel->interface_settings, "icon-theme", panel->current_icon_name);
  g_settings_set_string (panel->bg_settings, "picture-uri", panel->current_bg_name);

bail:
  g_list_free_full (list, (GDestroyNotify) gtk_tree_path_free);

return FALSE;
}

static void
cc_themes_panel_dispose (GObject *object)
{
  CcThemesPanel *panel = CC_THEMES_PANEL (object);

  if (panel->interface_settings)
    g_settings_set_string (panel->interface_settings, "icon-theme", panel->current_icon_name);
  if (panel->bg_settings)
    g_settings_set_string (panel->bg_settings, "picture-uri", panel->current_bg_name);

  g_clear_object (&panel->interface_settings);
  g_clear_object (&panel->wm_settings);
  g_clear_object (&panel->bg_settings);

  g_clear_object (&panel->builder);

  G_OBJECT_CLASS (cc_themes_panel_parent_class)->dispose (object);
}

static void
cc_themes_panel_class_init (CcThemesPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  CcPanelClass *panel_class = CC_PANEL_CLASS (klass);

  object_class->dispose = cc_themes_panel_dispose;

  panel_class->get_help_uri = cc_themes_panel_get_help_uri;
}

static void
cc_themes_panel_init (CcThemesPanel *panel)
{
  gchar *objects[] = {"themes-main-scrolled-window", NULL};
  g_autoptr(GError) err = NULL;
  GtkWidget *w;
  gint button_val;
  gdouble i_val;
  GtkWidget *themes_list_sw;
  GtkWidget *theme_apply_button;
  GtkWidget *current_theme_drawingarea;
  GtkTreeIter iter;

  g_resources_register (cc_themes_get_resource ());

  panel->builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (panel->builder,
                                         "/org/gnome/control-center/themes/themes.ui",
                                         objects, &err);
  if (err)
  {
    g_warning ("Could not load ui: %s", err->message);
    return;
  }

  panel->theme_apply_button = WID ("theme-apply-button");
  panel->themes_list_sw = WID ("themes-list-sw");

  panel->interface_settings = g_settings_new ("org.gnome.desktop.interface");
  panel->wm_settings = g_settings_new ("org.gnome.desktop.wm.preferences");
  panel->bg_settings = g_settings_new ("org.gnome.desktop.background");

  panel->themes_liststore = load_themes_liststore(panel);

  panel->current_icon_name = g_settings_get_string (panel->interface_settings, "icon-theme");
  panel->current_bg_name = g_settings_get_string (panel->bg_settings, "picture-uri");

// create icon view from list store
  panel->icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (panel->themes_liststore));
  gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (panel->icon_view),
                                    GTK_SELECTION_SINGLE);

  gtk_icon_view_set_text_column (GTK_ICON_VIEW (panel->icon_view), COL_ICON_NAME);

  gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (panel->icon_view), COL_THUMBNAIL);
  gtk_icon_view_set_columns (GTK_ICON_VIEW (panel->icon_view), 3);
  gtk_icon_view_set_item_width (GTK_ICON_VIEW (panel->icon_view), 120);
  gtk_icon_view_set_column_spacing (GTK_ICON_VIEW (panel->icon_view), 10);
  gtk_icon_view_set_margin (GTK_ICON_VIEW (panel->icon_view), 10);

  g_signal_connect (WID ("current-theme-drawingarea"), "draw", G_CALLBACK(on_draw_theme), panel);
  g_signal_connect (panel->theme_apply_button, "clicked", G_CALLBACK (on_clicked_apply), panel);
  g_signal_connect (panel->icon_view, "selection-changed",
                    G_CALLBACK (on_selection_theme), panel);

  gtk_scrolled_window_set_shadow_type (panel->themes_list_sw, GTK_SHADOW_ETCHED_IN);
  gtk_scrolled_window_set_policy (panel->themes_list_sw,
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);

  gtk_container_add (GTK_CONTAINER (panel->themes_list_sw), panel->icon_view);
  gtk_widget_grab_focus (panel->icon_view);

  w = WID ("themes-main-scrolled-window");
  gtk_container_add (GTK_CONTAINER (panel), w);
  gtk_widget_show_all (GTK_WIDGET (panel));
}
