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



/* Property identifiers */
enum 
{
  PROP_0,
  PROP_FILE,
};



static void tag_renamer_property_page_class_init   (TagRenamerPropertyPageClass *klass);
static void tag_renamer_property_page_init         (TagRenamerPropertyPage      *page);
static void tag_renamer_property_page_finalize     (GObject                     *object);
static void tag_renamer_property_page_get_property (GObject                     *object,
                                                    guint                        prop_id,
                                                    GValue                      *value,
                                                    GParamSpec                  *pspec);
static void tag_renamer_property_page_set_property (GObject                     *object,
                                                    guint                        prop_id,
                                                    const GValue                *value,
                                                    GParamSpec                  *pspec);
static void tag_renamer_property_page_file_changed (ThunarxFileInfo             *file,
                                                    TagRenamerPropertyPage      *page);



struct _TagRenamerPropertyPageClass
{
  ThunarxPropertyPageClass __parent__;
};



struct _TagRenamerPropertyPage
{
  ThunarxPropertyPage __parent__;

  /* Widgets */
  GtkTooltips     *tooltips;

  /* Properties */
  ThunarxFileInfo *file;
};




THUNARX_DEFINE_TYPE (TagRenamerPropertyPage, tag_renamer_property_page, THUNARX_TYPE_PROPERTY_PAGE);



static void
tag_renamer_property_page_class_init (TagRenamerPropertyPageClass *klass)
{
  GObjectClass             *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = tag_renamer_property_page_finalize;
  gobject_class->get_property = tag_renamer_property_page_get_property;
  gobject_class->set_property = tag_renamer_property_page_set_property;

  /**
   * TagRenamerPropertiesPage:file:
   *
   * The ThunarxFileInfo modified on this page.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FILE,
                                   g_param_spec_object ("file",
                                                        "file",
                                                        "file",
                                                        THUNARX_TYPE_FILE_INFO,
                                                        G_PARAM_READWRITE));
}



static void
tag_renamer_property_page_init (TagRenamerPropertyPage *page)
{
  GtkObject *adjustment;
  GtkWidget *table;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *spin;

  gtk_container_set_border_width (GTK_CONTAINER (page), 12);

  /* Allocate the shared tooltips */
  page->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (page->tooltips));

  table = gtk_table_new (2, 3, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_container_add (GTK_CONTAINER (page), table);
  gtk_widget_show (table);
  
  label = gtk_label_new (_("Artist:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Artist:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  // exo_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "artist");
  // g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (tag_rename_property_page_activate), page);
  // g_signal_connect (G_OBJECT (entry), "focus-out-event", G_CALLBACK (tag_rename_property_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter the name of the artist or author of this file here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
  gtk_widget_show (entry);

  label = gtk_label_new (_("Track:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Track:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  adjustment = gtk_adjustment_new (0, 1, 9999, 1, 5, 10);

  spin = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
  gtk_entry_set_activates_default (GTK_ENTRY (spin), TRUE);
  // exo_mutual_binding_new (G_OBJECT (spin), "value-changed", G_OBJECT (page), "track");
  // g_signal_connect (G_OBJECT (spin), "activate", G_CALLBACK (tag_rename_property_page_activate), page);
  // g_signal_connect (G_OBJECT (spin), "focus-out-event", G_CALLBACK (tag_rename_property_page_activate), page);
  // TODO Position the spin button on the left side of the table column
  gtk_tooltips_set_tip (page->tooltips, spin, _("Enter the track number here."), NULL);
  gtk_table_attach (GTK_TABLE (table), spin, 1, 2, 1, 2, GTK_SHRINK, 0, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin);
  gtk_widget_show (spin);
}



static void
tag_renamer_property_page_finalize (GObject *object)
{
  TagRenamerPropertyPage *page = TAG_RENAMER_PROPERTY_PAGE (object);
  
  /* Set the file object */
  tag_renamer_property_page_set_file (page, NULL);

  /* Release the tooltips */
  g_object_unref (G_OBJECT (page->tooltips));

  (*G_OBJECT_CLASS (tag_renamer_property_page_parent_class)->finalize) (object);
}



TagRenamerPropertyPage*
tag_renamer_property_page_new (void)
{
  TagRenamerPropertyPage *page = g_object_new (TYPE_TAG_RENAMER_PROPERTY_PAGE, NULL);

  thunarx_property_page_set_label (THUNARX_PROPERTY_PAGE (page), _("Audio Meta Tags"));

  return page;
}



static void
tag_renamer_property_page_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  TagRenamerPropertyPage *page = TAG_RENAMER_PROPERTY_PAGE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, tag_renamer_property_page_get_file (page));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
tag_renamer_property_page_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  TagRenamerPropertyPage *page = TAG_RENAMER_PROPERTY_PAGE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      tag_renamer_property_page_set_file (page, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



/**
 * tag_renamer_property_page_get_file:
 * @page : a #TagRenamerPropertyPage.
 *
 * Returns the current #ThunarxFileInfo 
 * for the @page.
 *
 * Return value: the file associated with this property page.
 **/
ThunarxFileInfo*
tag_renamer_property_page_get_file (TagRenamerPropertyPage *page)
{
  g_return_val_if_fail (IS_TAG_RENAMER_PROPERTY_PAGE (page), NULL);
  return page->file;
}



/**
 * tag_renamer_property_page_set_file:
 * @page : a #TagRenamerPropertyPage.
 * @file : a #ThunarxFileInfo
 * 
 * Sets the #ThunarxFileInfo for this @page.
 **/
void
tag_renamer_property_page_set_file (TagRenamerPropertyPage *page,
                                    ThunarxFileInfo        *file)
{
  g_return_if_fail (IS_TAG_RENAMER_PROPERTY_PAGE (page));
  g_return_if_fail (file == NULL || THUNARX_IS_FILE_INFO (file));

  /* Check if we already use this file */
  if (G_UNLIKELY (page->file == file))
    return;

  /* Disconnect from the previous file (if any) */
  if (G_LIKELY (page->file != NULL))
  {
    // g_signal_handlers_disconnect_by_func (G_OBJECT (page->file), tag_renamer_property_page_file_changed, page);
    g_object_unref (G_OBJECT (page->file));
  }

  /* Assign the value */
  page->file = file;

  /* Connect to the new file (if any) */
  if (G_LIKELY (file != NULL))
    {
      /* Take a reference on the info file */
      g_object_ref (G_OBJECT (page->file));
      
      tag_renamer_property_page_file_changed (file, page);
      
      g_signal_connect (G_OBJECT (file), "changed", G_CALLBACK (tag_renamer_property_page_file_changed), page);
    }
}



static void
tag_renamer_property_page_file_changed (ThunarxFileInfo        *file,
                                        TagRenamerPropertyPage *page)
{
  g_return_if_fail (THUNARX_IS_FILE_INFO (file));
  g_return_if_fail (IS_TAG_RENAMER_PROPERTY_PAGE (page));
  g_return_if_fail (page->file == file);

  /* Temporarily reset the file attribute */
  page->file = NULL;

  /* TODO Determine tags of this file */

  /* TODO Update the UI */

  /* Reset the file attribute */
  page->file = file;
}
