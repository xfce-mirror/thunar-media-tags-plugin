/* $Id$ */
/*-
 * Copyright (c) 2006 Jannis Pohlmann <jannis@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __THUNAR_SBR_TAG_RENAMER_H__
#define __THUNAR_SBR_TAG_RENAMER_H__

#include <thunarx/thunarx.h>

G_BEGIN_DECLS;

#define THUNAR_SBR_TYPE_TAG_RENAMER_SCHEME (thunar_sbr_tag_renamer_scheme_get_type ())

/**
 * ThunarSbrTagRenamerScheme:
 * @THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_ARTIST_SONG : <track-number>_-_<artist-name>_-_<song-name>.<extension>
 * 
 * The naming scheme for the #ThunarSbrTagRenamer.
 **/
typedef enum
{
  THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_TITLE,
  THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_ARTIST_TITLE,
  THUNAR_SBR_TAG_RENAMER_SCHEME_TITLE,
  THUNAR_SBR_TAG_RENAMER_SCHEME_ARTIST_TITLE,
  THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_TITLE,
  THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_ARTIST_TITLE,
} ThunarSbrTagRenamerScheme;

GType thunar_sbr_tag_renamer_scheme_get_type (void) G_GNUC_CONST G_GNUC_INTERNAL;
void thunar_sbr_register_tag_enum_types (ThunarxProviderPlugin *plugin) G_GNUC_INTERNAL;



typedef struct _ThunarSbrTagRenamerClass ThunarSbrTagRenamerClass;
typedef struct _ThunarSbrTagRenamer ThunarSbrTagRenamer;

#define THUNAR_SBR_TYPE_TAG_RENAMER            (thunar_sbr_tag_renamer_get_type ())
#define THUNAR_SBR_TAG_RENAMER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), THUNAR_SBR_TYPE_TAG_RENAMER, ThunarSbrTagRenamer))
#define THUNAR_SBR_TAG_RENAMER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), THUNAR_SBR_TYPE_TAG_RENAMER, ThunarSbrTagRenamerClass))
#define THUNAR_SBR_IS_TAG_RENAMER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THUNAR_SBR_TYPE_TAG_RENAMER))
#define THUNAR_SBR_IS_TAG_RENAMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), THUNAR_SBR_TYPE_TAG_RENAMER))
#define THUNAR_SBR_TAG_RENAMER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), THUNAR_SBR_TYPE_TAG_RENAMER, ThunarSbrTagRenamerClass))

GType                    thunar_sbr_tag_renamer_get_type            (void) G_GNUC_CONST G_GNUC_INTERNAL;
void                     thunar_sbr_tag_renamer_register_type       (ThunarxProviderPlugin    *plugin) G_GNUC_INTERNAL;

ThunarSbrTagRenamer      *thunar_sbr_tag_renamer_new                (void) G_GNUC_CONST G_GNUC_INTERNAL G_GNUC_MALLOC;

ThunarSbrTagRenamerScheme thunar_sbr_tag_renamer_get_scheme         (ThunarSbrTagRenamer      *tag_renamer) G_GNUC_INTERNAL;
void                      thunar_sbr_tag_renamer_set_scheme         (ThunarSbrTagRenamer      *tag_renamer,
                                                                     ThunarSbrTagRenamerScheme scheme) G_GNUC_INTERNAL;
gboolean                  thunar_sbr_tag_renamer_get_replace_spaces (ThunarSbrTagRenamer      *tag_renamer) G_GNUC_INTERNAL;
void                      thunar_sbr_tag_renamer_set_replace_spaces (ThunarSbrTagRenamer      *tag_renamer,
                                                                     gboolean                  replace) G_GNUC_INTERNAL;
gboolean                  thunar_sbr_tag_renamer_get_lowercase      (ThunarSbrTagRenamer      *tag_renamer) G_GNUC_INTERNAL;
void                      thunar_sbr_tag_renamer_set_lowercase      (ThunarSbrTagRenamer      *tag_renamer,
                                                                     gboolean                  lowercase) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__THUNAR_SBR_TAG_RENAMER_H__ */
