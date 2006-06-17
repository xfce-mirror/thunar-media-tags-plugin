/* $Id$ */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>
 *                    Jannis Pohlmann <jannis@xfce.org>
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

#ifndef __MEDIA_TAGS_PROVIDER_H__
#define __MEDIA_TAGS_PROVIDER_H__

#include <thunarx/thunarx.h>

G_BEGIN_DECLS;

typedef struct _MediaTagsProviderClass MediaTagsProviderClass;
typedef struct _MediaTagsProvider      MediaTagsProvider;

#define TYPE_MEDIA_TAGS_PROVIDER            (media_tags_provider_get_type ())
#define MEDIA_TAGS_PROVIDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_MEDIA_TAGS_PROVIDER, MediaTagsProvider))
#define MEDIA_TAGS_PROVIDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_MEDIA_TAGS_PROVIDER, MediaTagsProviderClass))
#define IS_MEDIA_TAGS_PROVIDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_MEDIA_TAGS_PROVIDER))
#define IS_MEDIA_TAGS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_MEDIA_TAGS_PROVIDER))
#define MEDIA_TAGS_PROVIDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_MEDIA_TAGS_PROVIDER, MediaTagsProviderClass))

GType    media_tags_provider_get_type        (void) G_GNUC_CONST G_GNUC_INTERNAL;
void     media_tags_provider_register_type   (ThunarxProviderPlugin *plugin) G_GNUC_INTERNAL;

gboolean media_tags_get_audio_file_supported (ThunarxFileInfo *info) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__MEDIA_TAGS_PROVIDER_H__ */
