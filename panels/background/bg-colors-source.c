/* bg-colors-source.c */
/*
 * Copyright (C) 2010 Intel, Inc
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
 * Author: Thomas Wood <thomas.wood@intel.com>
 *
 */

#include <config.h>
#include "bg-colors-source.h"

#include "cc-background-item.h"

#include <cairo-gobject.h>
#include <glib/gi18n-lib.h>
#include <gdesktop-enums.h>

struct _BgColorsSource
{
  BgSource parent_instance;
};

G_DEFINE_TYPE (BgColorsSource, bg_colors_source, BG_TYPE_SOURCE)

struct {
  GDesktopBackgroundShading type;
  int orientation;
  const char *pcolor;
} items[] = {
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#000000" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#db5d33" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#008094" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#5d479d" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#ab2876" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#fad166" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#437740" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#d272c4" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#ed9116" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#ff89a9" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#7a8aa2" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#888888" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#475b52" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#425265" },
  { G_DESKTOP_BACKGROUND_SHADING_SOLID, -1, "#7a634b" },
};

static gchar *
get_colors_path (void)
{
  return g_build_filename (g_get_user_config_dir (), "gnome-control-center", "backgrounds", "colors.ini", NULL);
}

static char *
get_colors_dir (void)
{
  return g_build_filename (g_get_user_config_dir (), "gnome-control-center", "backgrounds", NULL);
}

static void
bg_colors_source_add_color (BgColorsSource               *self,
                            GnomeDesktopThumbnailFactory *thumb_factory,
                            GtkListStore                 *store,
                            const char                   *color,
                            GtkTreeRowReference         **ret_row_ref)
{
  CcBackgroundItemFlags flags;
  g_autoptr(CcBackgroundItem) item = NULL;
  g_autoptr(GdkPixbuf) pixbuf = NULL;
  cairo_surface_t *surface;
  int scale_factor;
  int thumbnail_height, thumbnail_width;
  GtkTreeIter iter;

  thumbnail_height = bg_source_get_thumbnail_height (BG_SOURCE (self));
  thumbnail_width = bg_source_get_thumbnail_width (BG_SOURCE (self));

  item = cc_background_item_new (NULL);
  flags = CC_BACKGROUND_ITEM_HAS_PCOLOR |
          CC_BACKGROUND_ITEM_HAS_SCOLOR |
          CC_BACKGROUND_ITEM_HAS_SHADING |
          CC_BACKGROUND_ITEM_HAS_PLACEMENT |
          CC_BACKGROUND_ITEM_HAS_URI;
  /* It does have a URI, it's "none" */

  g_object_set (G_OBJECT (item),
                "uri", "file:///" DATADIR "/gnome-control-center/pixmaps/noise-texture-light.png",
                "primary-color", color,
                "secondary-color", color,
                "shading", G_DESKTOP_BACKGROUND_SHADING_SOLID,
                "placement", G_DESKTOP_BACKGROUND_STYLE_WALLPAPER,
                "flags", flags,
                NULL);
  cc_background_item_load (item, NULL);

  /* insert the item into the liststore */
  scale_factor = bg_source_get_scale_factor (BG_SOURCE (self));
  pixbuf = cc_background_item_get_thumbnail (item,
                                             thumb_factory,
                                             thumbnail_width, thumbnail_height,
                                             scale_factor);
  surface = gdk_cairo_surface_create_from_pixbuf (pixbuf, scale_factor, NULL);
  gtk_list_store_insert_with_values (store, &iter, 0,
                                     0, surface,
                                     1, item,
                                     -1);
  cairo_surface_destroy (surface);

  if (ret_row_ref)
    {
      GtkTreePath *path;

      path = gtk_tree_model_get_path (GTK_TREE_MODEL (store), &iter);
      *ret_row_ref = gtk_tree_row_reference_new (GTK_TREE_MODEL (store), path);
      gtk_tree_path_free (path);
    }
}

static void
bg_colors_source_constructed (GObject *object)
{
  BgColorsSource *self = BG_COLORS_SOURCE (object);
  g_autoptr(GnomeDesktopThumbnailFactory) thumb_factory = NULL;
  guint i;
  GtkListStore *store;
  g_autoptr(GKeyFile) keyfile = NULL;
  g_autofree gchar *path = NULL;

  G_OBJECT_CLASS (bg_colors_source_parent_class)->constructed (object);

  store = bg_source_get_liststore (BG_SOURCE (self));
  thumb_factory = gnome_desktop_thumbnail_factory_new (GNOME_DESKTOP_THUMBNAIL_SIZE_LARGE);

  for (i = 0; i < G_N_ELEMENTS (items); i++)
    {
      bg_colors_source_add_color (self, thumb_factory, store, items[i].pcolor, NULL);
    }

  keyfile = g_key_file_new ();
  path = get_colors_path ();
  if (g_key_file_load_from_file (keyfile, path, G_KEY_FILE_NONE, NULL))
    {
      g_auto(GStrv) colors = NULL;

      colors = g_key_file_get_string_list (keyfile, "Colors", "custom-colors", NULL, NULL);
      for (i = 0; colors != NULL && colors[i] != NULL; i++)
        {
          bg_colors_source_add_color (self, thumb_factory, store, colors[i], NULL);
        }
    }
}

gboolean
bg_colors_source_add (BgColorsSource       *self,
                      GdkRGBA              *rgba,
                      GtkTreeRowReference **ret_row_ref)
{
  g_autoptr(GnomeDesktopThumbnailFactory) thumb_factory = NULL;
  GtkListStore *store;
  g_autofree gchar *c = NULL;
  g_auto(GStrv) colors = NULL;
  gsize len;
  g_autoptr(GKeyFile) keyfile = NULL;
  g_autoptr(GError) error = NULL;
  g_autofree gchar *dir = NULL;
  g_autofree gchar *path = NULL;

  c = g_strdup_printf ("#%02x%02x%02x",
                       (int)(255*rgba->red),
                       (int)(255*rgba->green),
                       (int)(255*rgba->blue));

  thumb_factory = gnome_desktop_thumbnail_factory_new (GNOME_DESKTOP_THUMBNAIL_SIZE_LARGE);
  store = bg_source_get_liststore (BG_SOURCE (self));

  bg_colors_source_add_color (self, thumb_factory, store, c, ret_row_ref);

  /* Save to the keyfile */
  dir = get_colors_dir ();
  g_mkdir_with_parents (dir, 0700);

  path = get_colors_path ();
  colors = NULL;
  len = 0;

  keyfile = g_key_file_new ();
  if (g_key_file_load_from_file (keyfile, path, G_KEY_FILE_NONE, NULL))
    colors = g_key_file_get_string_list (keyfile, "Colors", "custom-colors", &len, NULL);

  if (len == 0 && colors != NULL)
    g_clear_pointer (&colors, g_strfreev);

  if (colors == NULL)
    {
      colors = g_new0 (char *, 2);
      colors[0] = g_steal_pointer (&c);
      len = 1;
    }
  else
    {
      char **new_colors;
      guint i;

      new_colors = g_new0 (char *, len + 2);
      for (i = 0; colors[i] != NULL; i++)
        {
          new_colors[i] = colors[i];
          colors[i] = NULL;
        }

      new_colors[len] = g_steal_pointer (&c);
      len++;

      g_strfreev (colors);
      colors = new_colors;
    }

  g_key_file_set_string_list (keyfile, "Colors", "custom-colors", (const gchar * const*) colors, len);

  if (!g_key_file_save_to_file (keyfile, path, &error))
      g_warning ("Could not save custom color: %s", error->message);

  return TRUE;
}

static void
bg_colors_source_init (BgColorsSource *self)
{
}

static void
bg_colors_source_class_init (BgColorsSourceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = bg_colors_source_constructed;
}

BgColorsSource *
bg_colors_source_new (GtkWindow *window)
{
  return g_object_new (BG_TYPE_COLORS_SOURCE, "window", window, NULL);
}

