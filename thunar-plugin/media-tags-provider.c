/* $Id: tag-renamer-provider.c 1218 2006-03-24 11:06:17Z jpohlmann $ */
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
  GList *pages = NULL;

  if (g_list_length (files) != 1) 
    return NULL;
  
#if 0
  gboolean supported = TRUE;

  GList *file = NULL;
  GList *supported_files = NULL;
  
  for (file = g_list_last (files); file != NULL; file = file->prev)
    {
      ThunarxFileInfo *info = THUNARX_FILE_INFO (file->data);
      
      gchar *uri = thunarx_file_info_get_uri (info);
      gchar *filename = g_filename_from_uri (uri, NULL, NULL);

      /* Try loading tag information */
      TagLib_File *taglib_file = taglib_file_new (filename);

      if (G_LIKELY (taglib_file != NULL))
        {
          /* Free the TagLib file instance */
          taglib_file_free (taglib_file);
          
          /* Add the file info to the supported list */
          supported_files = g_list_prepend (supported_files, info);
        }

      g_free (filename);
      g_free (uri);

      if (!supported)
        break;
    }
#endif

  GList* file = g_list_first (files);

  if (G_UNLIKELY (file == NULL))
    return NULL;

  ThunarxFileInfo *info = THUNARX_FILE_INFO (file->data);

  gchar *uri = thunarx_file_info_get_uri (info);
  gchar *filename = g_filename_from_uri (uri, NULL, NULL);

  /* Try loading the tag information */
  TagLib_File *taglib_file = taglib_file_new (filename);

  if (G_LIKELY (taglib_file != NULL))
    {
      /* Free the taglib file instance */
      taglib_file_free (taglib_file);

      /* Create the tag editor page */
      AudioTagsPage *page = audio_tags_page_new ();

      /* Assign supported file info to the page */
      audio_tags_page_set_file (page, info);
      
      /* Add the tag editor page to the pages provided by this plugin */
      pages = g_list_prepend (pages, page);
    }

  g_free (filename);
  g_free (uri);

  return pages;
}
