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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <taglib/tag_c.h>

#include <exo/exo.h>

#include <tag-renamer-property-page.h>



static void tag_renamer_property_page_class_init (TagRenamerPropertyPageClass *klass);
static void tag_renamer_property_page_init       (TagRenamerPropertyPage      *page);
static void tag_renamer_property_page_finalize   (GObject                     *object);



struct _TagRenamerPropertyPageClass
{
  ThunarxPropertyPageClass __parent__;
};



struct _TagRenamerPropertyPage
{
  ThunarxPropertyPage __parent__;
};




THUNARX_DEFINE_TYPE (TagRenamerPropertyPage, tag_renamer_property_page, THUNARX_TYPE_PROPERTY_PAGE);



static void
tag_renamer_property_page_class_init (TagRenamerPropertyPageClass *klass)
{
  GObjectClass             *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = tag_renamer_property_page_finalize;
}



static void
tag_renamer_property_page_init (TagRenamerPropertyPage *page)
{
  GtkWidget *label;
  label = gtk_label_new_with_mnemonic (_("ID3/Vorbis _Tags"));
  gtk_container_add (GTK_CONTAINER (page), label);
  gtk_widget_show (label);
}



static void
tag_renamer_property_page_finalize (GObject *object)
{
  (*G_OBJECT_CLASS (tag_renamer_property_page_parent_class)->finalize) (object);
}



TagRenamerPropertyPage*
tag_renamer_property_page_new (void)
{
  TagRenamerPropertyPage *page = g_object_new (TYPE_TAG_RENAMER_PROPERTY_PAGE, NULL);

  return page;
}
