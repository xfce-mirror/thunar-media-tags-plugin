/* $Id$ */
/*-
 * Copyright (c) 2006 Jannis Pohlmann <jannis@xfce.org>
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the 
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 * MA 02111-1307 USA
 */

#ifndef __TAG_RENAMER_H__
#define __TAG_RENAMER_H__

#include <thunarx/thunarx.h>

G_BEGIN_DECLS;

#define TYPE_TAG_RENAMER_FORMAT (tag_renamer_format_get_type ())

/**
 * TagRenamerFormat:
 * @TAG_RENAMER_FORMAT_TRACK_ARTIST_SONG : <track-number>_-_<artist-name>_-_<song-name>.<extension>
 * 
 * The naming format for the #TagRenamer.
 **/
typedef enum
{
  TAG_RENAMER_FORMAT_TITLE,
  TAG_RENAMER_FORMAT_ARTIST_TITLE,
  TAG_RENAMER_FORMAT_TRACK_TITLE,
  TAG_RENAMER_FORMAT_TRACK_ARTIST_TITLE,
  TAG_RENAMER_FORMAT_TRACK_DOT_TITLE,
  TAG_RENAMER_FORMAT_TRACK_DOT_ARTIST_TITLE,
  TAG_RENAMER_FORMAT_ARTIST_TRACK_TITLE,
} TagRenamerFormat;

GType tag_renamer_format_get_type     (void) G_GNUC_CONST G_GNUC_INTERNAL;
void  tag_renamer_register_enum_types (ThunarxProviderPlugin *plugin) G_GNUC_INTERNAL;



typedef struct _TagRenamerClass TagRenamerClass;
typedef struct _TagRenamer TagRenamer;

#define TYPE_TAG_RENAMER            (tag_renamer_get_type ())
#define TAG_RENAMER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TAG_RENAMER, TagRenamer))
#define TAG_RENAMER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TAG_RENAMER, TagRenamerClass))
#define IS_TAG_RENAMER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TAG_RENAMER))
#define IS_TAG_RENAMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TAG_RENAMER))
#define TAG_RENAMER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TAG_RENAMER, TagRenamerClass))

GType            tag_renamer_get_type           (void) G_GNUC_CONST G_GNUC_INTERNAL;
void             tag_renamer_register_type      (ThunarxProviderPlugin *plugin) G_GNUC_INTERNAL;

TagRenamer      *tag_renamer_new                (void) G_GNUC_CONST G_GNUC_INTERNAL G_GNUC_MALLOC;

TagRenamerFormat tag_renamer_get_format         (TagRenamer            *tag_renamer) G_GNUC_INTERNAL;
void             tag_renamer_set_format         (TagRenamer            *tag_renamer,
                                                 TagRenamerFormat       format) G_GNUC_INTERNAL;
gboolean         tag_renamer_get_replace_spaces (TagRenamer            *tag_renamer) G_GNUC_INTERNAL;
void             tag_renamer_set_replace_spaces (TagRenamer            *tag_renamer,
                                                 gboolean               replace) G_GNUC_INTERNAL;
gboolean         tag_renamer_get_lowercase      (TagRenamer            *tag_renamer) G_GNUC_INTERNAL;
void             tag_renamer_set_lowercase      (TagRenamer            *tag_renamer,
                                                 gboolean               lowercase) G_GNUC_INTERNAL;
const gchar     *tag_renamer_get_artist         (TagRenamer            *tag_renamer) G_GNUC_INTERNAL;
void             tag_renamer_set_artist         (TagRenamer            *tag_renamer,
                                                 const gchar           *artist) G_GNUC_INTERNAL;
const gchar     *tag_renamer_get_title          (TagRenamer            *tag_renamer) G_GNUC_INTERNAL;
void             tag_renamer_set_title          (TagRenamer            *tag_renamer,
                                                 const gchar           *title) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__TAG_RENAMER_H__ */
