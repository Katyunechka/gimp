/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * test-gimptilebackendtilemanager.c
 * Copyright (C) 2011 Martin Nordholts <martinn@src.gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gegl.h>
#include <gtk/gtk.h>
#include <string.h>

#include "widgets/widgets-types.h"

#include "base/tile-manager.h"
#include "base/tile-cache.h"

#include "gegl/gimptilebackendtilemanager.h"

#include "gegl/gimp-gegl-utils.h"

#include "tests.h"
#include "gimp-app-test-utils.h"


#define ADD_TEST(function) \
  g_test_add_func ("/gimptilebackendtilemanager/" #function, function);


/**
 * basic_usage:
 * @fixture:
 * @data:
 *
 * Test basic usage.
 **/
static void
basic_usage (void)
{
  GeglRectangle  rect                = { 0, 0, 10, 10 };
  GeglRectangle  pixel_rect          = { 5, 5, 1, 1 };
  guint16        opaque_magenta16[4] = { 0xffff, 0, 0xffff, 0xffff };
  GeglColor     *magenta             = gegl_color_new (NULL);

  TileManager     *tm;
  GeglTileBackend *backend;
  GeglBuffer      *buffer;
  guint16          actual_data[4];
  const Babl      *format = babl_format ("R'G'B'A u8");

  /* Write some pixels to the tile manager */
  tm = tile_manager_new (rect.width, rect.height, 4);

  buffer = gimp_tile_manager_create_buffer (tm, format);
  gegl_color_set_rgba (magenta, 1.0, 0.0, 1.0, 1.0);
  gegl_buffer_set_color (buffer, NULL, magenta);
  g_object_unref (magenta);
  g_object_unref (buffer);

  /* Make sure we can read them through the GeglBuffer using the
   * TileManager backend. Use u16 to complicate code paths, decreasing
   * risk of the test accidentally passing
   */
  backend = gimp_tile_backend_tile_manager_new (tm, format);
  buffer  = gegl_buffer_new_for_backend (NULL, backend);
  gegl_buffer_get (buffer,
                   &pixel_rect, 1.0 /*scale*/,
                   babl_format ("RGBA u16"), actual_data,
                   GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);
  g_assert_cmpint (0, ==, memcmp (opaque_magenta16, actual_data, sizeof (actual_data)));
}

int
main (int    argc,
      char **argv)
{
  g_type_init ();
  tile_cache_init (G_MAXUINT32);
  gegl_init (&argc, &argv);
  g_test_init (&argc, &argv, NULL);

  ADD_TEST (basic_usage);

  return g_test_run ();
}
