/* $Id$ */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>
 *                    Jannis Pohlmann <jannis@xfce.org>
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

#ifndef __TAG_RENAMER_PROVIDER_H__
#define __TAG_RENAMER_PROVIDER_H__

#include <thunarx/thunarx.h>

G_BEGIN_DECLS;

typedef struct _TagRenamerProviderClass TagRenamerProviderClass;
typedef struct _TagRenamerProvider      TagRenamerProvider;

#define TYPE_TAG_RENAMER_PROVIDER            (tag_renamer_provider_get_type ())
#define TAG_RENAMER_PROVIDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TAG_RENAMER_PROVIDER, TagRenamerProvider))
#define TAG_RENAMER_PROVIDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TAG_RENAMER_PROVIDER, TagRenamerProviderClass))
#define IS_TAG_RENAMER_PROVIDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TAG_RENAMER_PROVIDER))
#define IS_TAG_RENAMER_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TAG_RENAMER_PROVIDER))
#define TAG_RENAMER_PROVIDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TAG_RENAMER_PROVIDER, TagRenamerProviderClass))

GType tag_renamer_provider_get_type      (void) G_GNUC_CONST G_GNUC_INTERNAL;
void  tag_renamer_provider_register_type (ThunarxProviderPlugin *plugin) G_GNUC_INTERNAL;

G_END_DECLS;

#endif /* !__TAG_RENAMER_PROVIDER_H__ */
