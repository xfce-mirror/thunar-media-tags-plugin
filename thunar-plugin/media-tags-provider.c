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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <taglib/tag_c.h>

#include <media-tags-provider.h>
#include <tag-renamer.h>
#include <audio-tags-page.h>



static void   media_tags_provider_class_init                    (MediaTagsProviderClass           *klass);
static void   media_tags_provider_tag_renamer_provider_init     (ThunarxRenamerProviderIface      *iface);
static void   media_tags_provider_init                          (MediaTagsProvider                *sbr_provider);
static GList *media_tags_provider_get_renamers                  (ThunarxRenamerProvider           *renamer_provider);
static void   media_tags_provider_audio_tags_page_provider_init (ThunarxPropertyPageProviderIface *iface);
static GList *media_tags_provider_get_pages                     (ThunarxPropertyPageProvider      *renamer_provider,
                                                                 GList                            *files);



struct _MediaTagsProviderClass
{
  GObjectClass __parent__;
};

struct _MediaTagsProvider
{
  GObject __parent__;
};



THUNARX_DEFINE_TYPE_WITH_CODE (MediaTagsProvider,
                               media_tags_provider,
                               G_TYPE_OBJECT,
                               THUNARX_IMPLEMENT_INTERFACE (THUNARX_TYPE_RENAMER_PROVIDER,
                                                            media_tags_provider_tag_renamer_provider_init)
                               THUNARX_IMPLEMENT_INTERFACE (THUNARX_TYPE_PROPERTY_PAGE_PROVIDER,
                                                            media_tags_provider_audio_tags_page_provider_init));



static void
media_tags_provider_class_init (MediaTagsProviderClass *klass)
{
}



static void
media_tags_provider_tag_renamer_provider_init (ThunarxRenamerProviderIface *iface)
{
  iface->get_renamers = media_tags_provider_get_renamers;
}



static void
media_tags_provider_init (MediaTagsProvider *sbr_provider)
{
}



static GList*
media_tags_provider_get_renamers (ThunarxRenamerProvider *renamer_provider)
{
  GList *renamers = NULL;

  renamers = g_list_prepend (renamers, tag_renamer_new ());

  return renamers;
}



static void
media_tags_provider_audio_tags_page_provider_init (ThunarxPropertyPageProviderIface *iface)
{
  iface->get_pages = media_tags_provider_get_pages;
}



static GList*
media_tags_provider_get_pages (ThunarxPropertyPageProvider *page_provider, GList *files)
{
  GList           *pages = NULL;
  GList           *file;
  ThunarxFileInfo *info;

  if (g_list_length (files) != 1) 
    return NULL;
  
  file = g_list_first (files);

  if (G_UNLIKELY (file == NULL))
    return NULL;

  info = THUNARX_FILE_INFO (file->data);

  if (G_LIKELY (media_tags_get_audio_file_supported (info)))
    {
      AudioTagsPage *page;
      
      /* Create the tag editor page */
      page = audio_tags_page_new_with_save_button ();

      /* Assign supported file info to the page */
      audio_tags_page_set_file (page, info);
      
      /* Add the tag editor page to the pages provided by this plugin */
      pages = g_list_prepend (pages, page);
    }

  return pages;
}



gboolean
media_tags_get_audio_file_supported (ThunarxFileInfo *info)
{
  gchar       *uri;
  gchar       *filename;
  TagLib_File *taglib_file;
  gboolean     supported = FALSE;

  g_return_val_if_fail (info != NULL || THUNARX_IS_FILE_INFO (info), FALSE);
  
  uri = thunarx_file_info_get_uri (info);
  filename = g_filename_from_uri (uri, NULL, NULL);

  /* Try loading the tag information */
  taglib_file = taglib_file_new (filename);

  if (G_LIKELY (taglib_file != NULL))
    {
      /* Free the taglib file */
      taglib_file_free (taglib_file);

      supported = TRUE;
    }
      

  g_free (filename);
  g_free (uri);

  return supported;
}
