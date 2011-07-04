/*
    This file is part of darktable,
    copyright (c) 2011 henrik andersson.

    darktable is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    darktable is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with darktable.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "common/darktable.h"
#include "common/collection.h"
#include "common/debug.h"
#include "common/image_cache.h"
#include "control/control.h"
#include "control/conf.h"
#include "gui/gtk.h"


void dt_ratings_apply_to_selection (int rating)
{
  uint32_t count = dt_collection_get_selected_count(darktable.collection);
  if (count)
  {
    dt_control_log(_("applying rating %d onto %d image(s)"),rating,count);
#if 0 // not updating cache
    gchar query[1024]={0};
    g_snprintf(query,1024,
	       "update images set flags=(images.flags & ~7) | (7 & %d) where id in (select imgid from selected_images)",
	       rating
	       );
    DT_DEBUG_SQLITE3_EXEC(darktable.db, query, NULL, NULL, NULL);
#endif

    /* for each selected image update rating */
    sqlite3_stmt *stmt;
    DT_DEBUG_SQLITE3_PREPARE_V2(darktable.db, "select imgid from selected_images", -1, &stmt, NULL);
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
      dt_image_t *image = dt_image_cache_get(sqlite3_column_int(stmt, 0), 'r');
      image->flags = (image->flags & ~0x7) | (0x7 & rating);
      dt_image_cache_flush(image);
      dt_image_cache_release(image, 'r');
    }
    sqlite3_finalize(stmt);

    /* redraw view */
    dt_control_queue_draw_all();
  }
  else
    dt_control_log(_("no images selected for applying rating"));

}

