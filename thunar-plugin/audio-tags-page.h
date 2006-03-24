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

#ifndef __AUDIO_TAGS_PAGE_H__
#define __AUDIO_TAGS_PAGE_H__

#include <thunarx/thunarx.h>
#include <taglib/tag_c.h>

G_BEGIN_DECLS;

typedef struct _AudioTagsPageClass AudioTagsPageClass;
typedef struct _AudioTagsPage      AudioTagsPage;

#define TYPE_AUDIO_TAGS_PAGE            (audio_tags_page_get_type ())
#define AUDIO_TAGS_PAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUDIO_TAGS_PAGE, AudioTagsPage))
#define AUDIO_TAGS_PAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_AUDIO_TAGS_PAGE, AudioTagsPageClass))
#define IS_AUDIO_TAGS_PAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUDIO_TAGS_PAGE))
#define IS_AUDIO_TAGS_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_AUDIO_TAGS_PAGE))
#define AUDIO_TAGS_PAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_AUDIO_TAGS_PAGE, AudioTagsPageClass))

GType            audio_tags_page_get_type        (void) G_GNUC_CONST G_GNUC_INTERNAL;
void             audio_tags_page_register_type   (ThunarxProviderPlugin *plugin) G_GNUC_INTERNAL;

AudioTagsPage   *audio_tags_page_new             (void) G_GNUC_CONST G_GNUC_INTERNAL G_GNUC_MALLOC;

ThunarxFileInfo *audio_tags_page_get_file        (AudioTagsPage         *page) G_GNUC_INTERNAL;
void             audio_tags_page_set_file        (AudioTagsPage         *page,
                                                  ThunarxFileInfo       *file) G_GNUC_INTERNAL;
TagLib_File     *audio_tags_page_get_taglib_file (AudioTagsPage         *page) G_GNUC_INTERNAL;
void             audio_tags_page_set_taglib_file (AudioTagsPage         *page,
                                                  TagLib_File           *file) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__AUDIO_TAGS_PAGE_H__ */
