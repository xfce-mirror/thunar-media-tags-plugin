/* $Id: audio-tags-page.c 1218 2006-03-24 11:06:17Z jpohlmann $ */
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

#include <audio-tags-page.h>



/* Property identifiers */
enum 
{
  PROP_0,
  PROP_FILE,
  PROP_TRACK,
};



static void audio_tags_page_class_init   (AudioTagsPageClass *klass);
static void audio_tags_page_init         (AudioTagsPage      *page);
static void audio_tags_page_finalize     (GObject            *object);
static void audio_tags_page_get_property (GObject            *object,
                                          guint               prop_id,
                                          GValue             *value,
                                          GParamSpec         *pspec);
static void audio_tags_page_set_property (GObject            *object,
                                          guint               prop_id,
                                          const GValue       *value,
                                          GParamSpec         *pspec);
static void audio_tags_page_file_changed (ThunarxFileInfo    *file,
                                          AudioTagsPage      *page);
static gboolean audio_tags_page_activate (AudioTagsPage      *page);



struct _AudioTagsPageClass
{
  ThunarxPropertyPageClass __parent__;
};



struct _AudioTagsPage
{
  ThunarxPropertyPage __parent__;

  /* Widgets */
  GtkTooltips     *tooltips;

  /* Properties */
  ThunarxFileInfo *file;
  guint            track;
};




THUNARX_DEFINE_TYPE (AudioTagsPage, audio_tags_page, THUNARX_TYPE_PROPERTY_PAGE);



static void
audio_tags_page_class_init (AudioTagsPageClass *klass)
{
  GObjectClass             *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = audio_tags_page_finalize;
  gobject_class->get_property = audio_tags_page_get_property;
  gobject_class->set_property = audio_tags_page_set_property;

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

  /**
   * TagRenamerPropertiesPage:track:
   *
   * The track number entered by the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TRACK,
                                   g_param_spec_double ("track",
                                                        "track",
                                                        "track",
                                                        1,
                                                        999,
                                                        1,
                                                        G_PARAM_READWRITE));
}



static void
audio_tags_page_init (AudioTagsPage *page)
{
  GtkObject *adjustment;
  GtkWidget *vbox;
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *alignment;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *combo;
  GtkWidget *spin;

  gtk_container_set_border_width (GTK_CONTAINER (page), 12);

  /* Allocate the shared tooltips */
  page->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (page->tooltips));

  /* Main container */
  vbox = gtk_vbox_new (FALSE, 12);
  gtk_container_add (GTK_CONTAINER (page), vbox);
  gtk_widget_show (vbox);

  /* File info frame */
  frame = gtk_frame_new (_("Info"));
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, TRUE, 0);
  gtk_widget_show (frame);

  /* Tag editor frame */
  frame = gtk_frame_new (_("Tags"));
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, TRUE, 0);
  gtk_widget_show (frame);

  /* Tag editor layout table */
  table = gtk_table_new (6, 4, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_container_add (GTK_CONTAINER (frame), table);
  gtk_container_set_border_width (GTK_CONTAINER (table), 12);
  gtk_widget_show (table);

  /* Track label */
  label = gtk_label_new (_("Track:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Track:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Track spin button alignment */
  alignment = gtk_alignment_new (0.0, 0.5, 0.0, 0.0);
  gtk_table_attach (GTK_TABLE (table), alignment, 1, 2, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (alignment);

  /* Track adjustment */
  adjustment = gtk_adjustment_new (1, 1, 999, 1, 5, 10);

  /* Track spin button */
  spin = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
  gtk_entry_set_activates_default (GTK_ENTRY (spin), TRUE);
  exo_binding_new (G_OBJECT (adjustment), "value", G_OBJECT (page), "track");
  g_signal_connect (G_OBJECT (spin), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect (G_OBJECT (spin), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, spin, _("Enter the track number here."), NULL);
  gtk_container_add (GTK_CONTAINER (alignment), spin);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin);
  gtk_widget_show (spin);
  
  /* Artist label */
  label = gtk_label_new (_("Artist:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Artist:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Artist entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  // exo_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "artist");
  // g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  // g_signal_connect (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter the name of the artist or author of this file here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
  gtk_widget_show (entry);
  
  /* Title label */
  label = gtk_label_new (_("Title:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Title:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Title entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  // exo_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "title");
  // g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  // g_signal_connect (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter the song title here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, 1, 2, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_widget_show (entry);

  /* Album label */
  label = gtk_label_new (_("Album:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Album:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Album entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  // exo_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "album");
  // g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  // g_signal_connect (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter the album/record title here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, 2, 3, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_widget_show (entry);
  
  /* Comment label */
  label = gtk_label_new (_("Comment:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Comment:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Comment entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  // exo_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "album");
  // g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  // g_signal_connect (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter your comments here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_widget_show (entry);

  /* Year label */
  label = gtk_label_new (_("Year:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Year:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 4, 5, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Year spin button alignment */
  alignment = gtk_alignment_new (0.0, 0.5, 0.0, 0.0);
  gtk_table_attach (GTK_TABLE (table), alignment, 1, 2, 4, 5, GTK_FILL, 0, 0, 0);
  gtk_widget_show (alignment);

  /* Year spin button range description */
  adjustment = gtk_adjustment_new (1700, 2006, 9999, 1, 10, 100);

  /* Year spin button */
  spin = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
  gtk_entry_set_activates_default (GTK_ENTRY (spin), TRUE);
  // exo_binding_new (G_OBJECT (spin), "value-changed", G_OBJECT (page), "year");
  // g_signal_connect (G_OBJECT (spin), "activate", G_CALLBACK (audio_tags_page_activate), page);
  // g_signal_connect (G_OBJECT (spin), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, spin, _("Enter the release year here."), NULL);
  gtk_container_add (GTK_CONTAINER (alignment), spin);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin);
  gtk_widget_show (spin);
  
  /* Genre label */
  label = gtk_label_new (_("Genre:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Genre:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, 4, 5, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Genre combo box */
  combo = gtk_combo_box_entry_new_text ();
  // exo_binding_new (G_OBJECT (GTK_BIN (combo)->child), "text", G_OBJECT (page), "album");
  // g_signal_connect (G_OBJECT (GTK_BIN (combo)->child), "activate", G_CALLBACK (audio_tags_page_activate), page);
  // g_signal_connect (G_OBJECT (GTK_BIN (entry)->child), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, combo, _("Select or enter the genre of this song here."), NULL);
  gtk_table_attach (GTK_TABLE (table), combo, 3, 4, 4, 5, GTK_FILL, 0, 0, 0);
  gtk_widget_show (combo);

  /* Default genre list */
  static const gchar *genres[] = {
    "A Cappella", "Acid", "Acid Jazz", "Acid Punk", "Acoustic", "Alt. Rock",
    "Alternative", "Ambient", "Anime", "Avantgarde", "Ballad", "Bass",
    "Beat", "Bebob", "Big Band", "Black Metal", "Bluegrass", "Blues",
    "Booty Bass", "BritPop", "Cabaret", "Celtic", "Chamber music", "Chanson",
    "Chorus", "Christian Gangsta Rap", "Christian Rap", "Christian Rock", "Classic Rock", "Classical",
    "Club", "Club-House", "Comedy", "Contemporary Christian", "Country", "Crossover",
    "Cult", "Dance", "Dance Hall", "Darkwave", "Death metal", "Disco",
    "Dream", "Drum & Bass", "Drum Solo", "Duet", "Easy Listening", "Electronic",
    "Ethnic", "Euro-House", "Euro-Techno", "Eurodance", "Fast Fusion", "Folk",
    "Folk-Rock", "Folklore", "Freestyle", "Funk", "Fusion", "Game",
    "Gangsta", "Goa", "Gospel", "Gothic", "Gothic Rock", "Grunge",
    "Hard Rock", "Hardcore", "Heavy Metal", "Hip-Hop", "House", "Humour",
    "Indie", "Industrial", "Instrumental", "Instrumental pop", "Instrumental rock", "JPop",
    "Jazz", "Jazz+Funk", "Jungle", "Latin", "Lo-Fi", "Meditative",
    "Merengue", "Metal", "Musical", "National Folk", "Native American", "Negerpunk",
    "New Age", "New Wave", "Noise", "Oldies", "Opera", "Other",
    "Polka", "Polsk Punk", "Pop", "Pop-Folk", "Pop/Funk", "Porn Groove",
    "Power Ballad", "Pranks", "Primus", "Progressive Rock", "Psychedelic", "Psychedelic Rock",
    "Punk", "Punk Rock", "R&B", "Rap", "Rave", "Reggae",
    "Retro", "Revival", "Rhythmic soul", "Rock", "Rock & Roll", "Salsa",
    "Samba", "Satire", "Showtunes", "Ska", "Slow Jam", "Slow Rock",
    "Sonata", "Soul", "Sound Clip", "Soundtrack", "Southern Rock", "Space",
    "Speech", "Swing", "Symphonic Rock", "Symphony", "Synthpop", "Tango",
    "Techno", "Techno-Industrial", "Terror", "Thrash Metal", "Top 40", "Trailer",
  };

  int i;
  for (i=0; i<G_N_ELEMENTS (genres); i++) 
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), genres[i]);
}



static void
audio_tags_page_finalize (GObject *object)
{
  AudioTagsPage *page = AUDIO_TAGS_PAGE (object);
  
  /* Set the file object */
  audio_tags_page_set_file (page, NULL);

  /* Release the tooltips */
  g_object_unref (G_OBJECT (page->tooltips));

  (*G_OBJECT_CLASS (audio_tags_page_parent_class)->finalize) (object);
}



AudioTagsPage*
audio_tags_page_new (void)
{
  AudioTagsPage *page = g_object_new (TYPE_AUDIO_TAGS_PAGE, NULL);

  thunarx_property_page_set_label (THUNARX_PROPERTY_PAGE (page), _("Audio Meta Tags"));

  return page;
}



static void
audio_tags_page_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  AudioTagsPage *page = AUDIO_TAGS_PAGE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, audio_tags_page_get_file (page));
      break;

    case PROP_TRACK:
      g_value_set_double (value, page->track);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
audio_tags_page_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  AudioTagsPage *page = AUDIO_TAGS_PAGE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      audio_tags_page_set_file (page, g_value_get_object (value));
      break;

    case PROP_TRACK:
      page->track = g_value_get_double (value);
      break;
      
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



/**
 * audio_tags_page_get_file:
 * @page : a #AudioTagsPage.
 *
 * Returns the current #ThunarxFileInfo 
 * for the @page.
 *
 * Return value: the file associated with this property page.
 **/
ThunarxFileInfo*
audio_tags_page_get_file (AudioTagsPage *page)
{
  g_return_val_if_fail (IS_AUDIO_TAGS_PAGE (page), NULL);
  return page->file;
}



/**
 * audio_tags_page_set_file:
 * @page : a #AudioTagsPage.
 * @file : a #ThunarxFileInfo
 * 
 * Sets the #ThunarxFileInfo for this @page.
 **/
void
audio_tags_page_set_file (AudioTagsPage   *page,
                          ThunarxFileInfo *file)
{
  g_return_if_fail (IS_AUDIO_TAGS_PAGE (page));
  g_return_if_fail (file == NULL || THUNARX_IS_FILE_INFO (file));

  /* Check if we already use this file */
  if (G_UNLIKELY (page->file == file))
    return;

  /* Disconnect from the previous file (if any) */
  if (G_LIKELY (page->file != NULL))
  {
    // g_signal_handlers_disconnect_by_func (G_OBJECT (page->file), audio_tags_page_file_changed, page);
    g_object_unref (G_OBJECT (page->file));
  }

  /* Assign the value */
  page->file = file;

  /* Connect to the new file (if any) */
  if (G_LIKELY (file != NULL))
    {
      /* Take a reference on the info file */
      g_object_ref (G_OBJECT (page->file));
      
      audio_tags_page_file_changed (file, page);
      
      g_signal_connect (G_OBJECT (file), "changed", G_CALLBACK (audio_tags_page_file_changed), page);
    }
}



static void
audio_tags_page_file_changed (ThunarxFileInfo *file,
                              AudioTagsPage   *page)
{
  g_return_if_fail (THUNARX_IS_FILE_INFO (file));
  g_return_if_fail (IS_AUDIO_TAGS_PAGE (page));
  g_return_if_fail (page->file == file);

  /* Temporarily reset the file attribute */
  page->file = NULL;

  /* TODO Determine tags of this file */

  /* TODO Update the UI */

  /* Reset the file attribute */
  page->file = file;
}



static gboolean
audio_tags_page_activate (AudioTagsPage *page)
{
  g_print ("%d\n", page->track);
  return FALSE;
}
