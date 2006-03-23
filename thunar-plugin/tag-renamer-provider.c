/* $Id: tag-renamer-provider.c 1205 2006-03-23 12:43:58Z jpohlmann $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tag-renamer-provider.h>
#include <tag-renamer.h>



static void   tag_renamer_provider_class_init            (TagRenamerProviderClass     *klass);
static void   tag_renamer_provider_renamer_provider_init (ThunarxRenamerProviderIface *iface);
static void   tag_renamer_provider_init                  (TagRenamerProvider          *sbr_provider);
static GList *tag_renamer_provider_get_renamers          (ThunarxRenamerProvider      *renamer_provider);



struct _TagRenamerProviderClass
{
  GObjectClass __parent__;
};

struct _TagRenamerProvider
{
  GObject __parent__;
};



THUNARX_DEFINE_TYPE_WITH_CODE (TagRenamerProvider,
                               tag_renamer_provider,
                               G_TYPE_OBJECT,
                               THUNARX_IMPLEMENT_INTERFACE (THUNARX_TYPE_RENAMER_PROVIDER,
                                                            tag_renamer_provider_renamer_provider_init));



static void
tag_renamer_provider_class_init (TagRenamerProviderClass *klass)
{
}



static void
tag_renamer_provider_renamer_provider_init (ThunarxRenamerProviderIface *iface)
{
  iface->get_renamers = tag_renamer_provider_get_renamers;
}



static void
tag_renamer_provider_init (TagRenamerProvider *sbr_provider)
{
}



static GList*
tag_renamer_provider_get_renamers (ThunarxRenamerProvider *renamer_provider)
{
  GList *renamers = NULL;

  renamers = g_list_prepend (renamers, tag_renamer_new ());

  return renamers;
}






