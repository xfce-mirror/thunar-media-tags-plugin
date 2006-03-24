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

#include <audio-tags-page.h>



/* Property identifiers */
enum 
{
  PROP_0,
  PROP_FILE,
  PROP_TAGLIB_FILE,
  PROP_TRACK,
  PROP_ARTIST,
  PROP_TITLE,
  PROP_ALBUM,
  PROP_COMMENT,
  PROP_GENRE,
  PROP_YEAR,
};



static void     audio_tags_page_class_init          (AudioTagsPageClass *klass);
static void     audio_tags_page_init                (AudioTagsPage      *page);
static void     audio_tags_page_finalize            (GObject            *object);
static void     audio_tags_page_get_property        (GObject            *object,
                                                     guint               prop_id,
                                                     GValue             *value,
                                                     GParamSpec         *pspec);
static void     audio_tags_page_set_property        (GObject            *object,
                                                     guint               prop_id,
                                                     const GValue       *value,
                                                     GParamSpec         *pspec);
static void     audio_tags_page_file_changed        (ThunarxFileInfo    *file,
                                                     AudioTagsPage      *page);
static void     audio_tags_page_taglib_file_changed (TagLib_File        *taglib_file,
                                                     AudioTagsPage      *page);
static gboolean audio_tags_page_activate            (AudioTagsPage      *page);
static gboolean audio_tags_page_load_tags           (AudioTagsPage      *page);



struct _AudioTagsPageClass
{
  ThunarxPropertyPageClass __parent__;
};



struct _AudioTagsPage
{
  ThunarxPropertyPage __parent__;

  /* Widgets */
  GtkTooltips     *tooltips;

  /* Timeouts */
  guint            changed_timeout;

  /* Properties */
  ThunarxFileInfo *file;
  TagLib_File     *taglib_file;
  guint            track;
  gchar           *artist;
  gchar           *title;
  gchar           *album;
  gchar           *comment;
  gchar           *genre;
  guint            year;
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
   * The #ThunarxFileInfo modified on this page.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FILE,
                                   g_param_spec_object ("file",
                                                        "file",
                                                        "file",
                                                        THUNARX_TYPE_FILE_INFO,
                                                        G_PARAM_READWRITE));

  /**
   * TagRenamerPropertiesPage:taglib-file:
   *
   * The #TagLib_File used on this page.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TAGLIB_FILE,
                                   g_param_spec_pointer ("taglib-file",
                                                         "taglib-file",
                                                         "taglib-file",
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
  
  /**
   * TagRenamerPropertiesPage:artist:
   *
   * The artist entered by the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ARTIST,
                                   g_param_spec_string ("artist",
                                                        "artist",
                                                        "artist",
                                                        _("Unknown Artist"),
                                                        G_PARAM_READWRITE));

  /**
   * TagRenamerPropertiesPage:album:
   *
   * The album title entered by the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ALBUM,
                                   g_param_spec_string ("album",
                                                        "album",
                                                        "album",
                                                        _("Unknown Album"),
                                                        G_PARAM_READWRITE));

  /**
   * TagRenamerPropertiesPage:title:
   *
   * The song title entered by the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TITLE,
                                   g_param_spec_string ("title",
                                                        "title",
                                                        "title",
                                                        _("Unknown Title"),
                                                        G_PARAM_READWRITE));

  /**
   * TagRenamerPropertiesPage:comment:
   *
   * The comment entered by the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COMMENT,
                                   g_param_spec_string ("comment",
                                                        "comment",
                                                        "comment",
                                                        "",
                                                        G_PARAM_READWRITE));

  /**
   * TagRenamerPropertiesPage:genre:
   *
   * The music genre entered by the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_GENRE,
                                   g_param_spec_string ("genre",
                                                        "genre",
                                                        "genre",
                                                        "",
                                                        G_PARAM_READWRITE));

  /**
   * TagRenamerPropertiesPage:year:
   *
   * The year entered by the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_YEAR,
                                   g_param_spec_double ("year",
                                                        "year",
                                                        "year",
                                                        1700,
                                                        9999,
                                                        2006,
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

  gtk_container_set_border_width (GTK_CONTAINER (page), 8);

  /* Allocate the shared tooltips */
  page->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (page->tooltips));

  /* Main container */
  vbox = gtk_vbox_new (FALSE, 8);
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
  adjustment = gtk_adjustment_new (0, 0, 999, 1, 5, 10);

  /* Track spin button */
  spin = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
  gtk_entry_set_activates_default (GTK_ENTRY (spin), TRUE);
  exo_mutual_binding_new (G_OBJECT (adjustment), "value", G_OBJECT (page), "track");
  g_signal_connect_swapped (G_OBJECT (spin), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect_swapped (G_OBJECT (spin), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, spin, _("Enter the track number here."), NULL);
  gtk_container_add (GTK_CONTAINER (alignment), spin);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin);
  gtk_widget_show (spin);
  
  /* Year label */
  label = gtk_label_new (_("Year:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Year:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Year spin button alignment */
  alignment = gtk_alignment_new (0.0, 0.5, 0.0, 0.0);
  gtk_table_attach (GTK_TABLE (table), alignment, 3, 4, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (alignment);

  /* Year spin button range description */
  adjustment = gtk_adjustment_new (2006, 1700, 9999, 1, 10, 100);

  /* Year spin button */
  spin = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
  gtk_entry_set_activates_default (GTK_ENTRY (spin), TRUE);
  exo_mutual_binding_new (G_OBJECT (adjustment), "value", G_OBJECT (page), "year");
  g_signal_connect_swapped (G_OBJECT (spin), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect_swapped (G_OBJECT (spin), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, spin, _("Enter the release year here."), NULL);
  gtk_container_add (GTK_CONTAINER (alignment), spin);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), spin);
  gtk_widget_show (spin);
  
  /* Artist label */
  label = gtk_label_new (_("Artist:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Artist:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Artist entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "artist");
  g_signal_connect_swapped (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect_swapped (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter the name of the artist or author of this file here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, 1, 2, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
  gtk_widget_show (entry);
  
  /* Title label */
  label = gtk_label_new (_("Title:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Title:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Title entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "title");
  g_signal_connect_swapped (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect_swapped (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter the song title here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, 2, 3, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_widget_show (entry);

  /* Album label */
  label = gtk_label_new (_("Album:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Album:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Album entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "album");
  g_signal_connect_swapped (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect_swapped (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter the album/record title here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_widget_show (entry);
  
  /* Comment label */
  label = gtk_label_new (_("Comment:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Comment:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 4, 5, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Comment entry */
  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (entry), "text", G_OBJECT (page), "comment");
  g_signal_connect_swapped (G_OBJECT (entry), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect_swapped (G_OBJECT (entry), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, entry, _("Enter your comments here."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_widget_show (entry);

  /* Genre label */
  label = gtk_label_new (_("Genre:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0f, 0.5f);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Genre:</b>"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 5, 6, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Genre combo box */
  combo = gtk_combo_box_entry_new_text ();
  exo_mutual_binding_new (G_OBJECT (GTK_BIN (combo)->child), "text", G_OBJECT (page), "genre");
  g_signal_connect_swapped (G_OBJECT (GTK_BIN (combo)->child), "activate", G_CALLBACK (audio_tags_page_activate), page);
  g_signal_connect_swapped (G_OBJECT (GTK_BIN (combo)->child), "focus-out-event", G_CALLBACK (audio_tags_page_activate), page);
  gtk_tooltips_set_tip (page->tooltips, combo, _("Select or enter the genre of this song here."), NULL);
  gtk_table_attach (GTK_TABLE (table), combo, 1, 4, 5, 6, GTK_FILL, 0, 0, 0);
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

  /* Unregister the changed_timeout */
  if (G_UNLIKELY (page->changed_timeout != 0))
    g_source_remove (page->changed_timeout);
  
  /* Free file reference */
  audio_tags_page_set_file (page, NULL);

  /* Free taglib file reference */
  audio_tags_page_set_taglib_file (page, NULL);

  /* Free strings */
  if (G_LIKELY (page->artist != NULL))
    g_free (page->artist);

  if (G_LIKELY (page->title != NULL))
    g_free (page->title);

  if (G_LIKELY (page->album != NULL))
    g_free (page->album);

  if (G_LIKELY (page->comment != NULL))
    g_free (page->comment);

  if (G_LIKELY (page->genre != NULL))
    g_free (page->genre);

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

    case PROP_TAGLIB_FILE:
      g_value_set_pointer (value, audio_tags_page_get_taglib_file (page));
      break;

    case PROP_TRACK:
      g_value_set_double (value, page->track);
      break;

    case PROP_ARTIST:
      g_value_set_string (value, page->artist);
      break;

    case PROP_TITLE:
      g_value_set_string (value, page->title);
      break;

    case PROP_ALBUM:
      g_value_set_string (value, page->album);
      break;

    case PROP_COMMENT:
      g_value_set_string (value, page->comment);
      break;

    case PROP_GENRE:
      g_value_set_string (value, page->genre);
      break;
      
    case PROP_YEAR:
      g_value_set_double (value, page->year);
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

    case PROP_TAGLIB_FILE:
      audio_tags_page_set_taglib_file (page, g_value_get_pointer (value));
      break;
      
    case PROP_TRACK:
      page->track = g_value_get_double (value);
      break;

    case PROP_ARTIST:
      if (G_LIKELY (page->artist != NULL))
        g_free (page->artist);
      page->artist = g_strstrip (g_strdup (g_value_get_string (value)));
      break;

    case PROP_TITLE:
      if (G_LIKELY (page->title != NULL))
        g_free (page->title);
      page->title = g_strstrip (g_strdup (g_value_get_string (value)));
      break;
      
    case PROP_ALBUM:
      if (G_LIKELY (page->album != NULL))
        g_free (page->album);
      page->album = g_strstrip (g_strdup (g_value_get_string (value)));
      break;
      
    case PROP_COMMENT:
      if (G_LIKELY (page->comment != NULL))
        g_free (page->comment);
      page->comment = g_strstrip (g_strdup (g_value_get_string (value)));
      break;
      
    case PROP_GENRE:
      if (G_LIKELY (page->genre != NULL))
        g_free (page->genre);
      page->genre = g_strstrip (g_strdup (g_value_get_string (value)));
      break;
      
    case PROP_YEAR:
      page->year = g_value_get_double (value);
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
    g_signal_handlers_disconnect_by_func (G_OBJECT (page->file), audio_tags_page_file_changed, page);
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



/**
 * audio_tags_page_get_taglib_file:
 * @page : a #AudioTagsPage.
 *
 * Returns the current #TagLib_File 
 * for the @page.
 *
 * Return value: the taglib file associated with this property page.
 **/
TagLib_File*
audio_tags_page_get_taglib_file (AudioTagsPage *page)
{
  g_return_val_if_fail (IS_AUDIO_TAGS_PAGE (page), NULL);
  return page->taglib_file;
}



/**
 * audio_tags_page_set_taglib_file:
 * @page : a #AudioTagsPage.
 * @file : a #TagLib_File
 * 
 * Sets the #TagLib_File for this @page.
 **/
void
audio_tags_page_set_taglib_file (AudioTagsPage   *page,
                                 TagLib_File     *taglib_file)
{
  g_return_if_fail (IS_AUDIO_TAGS_PAGE (page));

  /* Check if we already use this file */
  if (G_UNLIKELY (page->taglib_file == taglib_file))
    return;

  /* Destroy the current object */
  if (G_LIKELY (page->taglib_file != NULL))
      taglib_file_free (page->taglib_file);

  /* Assign the value */
  page->taglib_file = taglib_file;

  /* Connect to the new file (if any) */
  if (G_LIKELY (taglib_file != NULL))
      audio_tags_page_taglib_file_changed (taglib_file, page);
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

  /* Make page sensitive again */
  gtk_widget_set_sensitive (GTK_WIDGET (page), TRUE);

  /* Load tag information after 250 ms */
  if (page->changed_timeout <= 0)
      page->changed_timeout = g_timeout_add_full (G_PRIORITY_DEFAULT_IDLE, 250, (GSourceFunc)audio_tags_page_load_tags, page, NULL);

  /* Reset the file attribute */
  page->file = file;
}



static void
audio_tags_page_taglib_file_changed (TagLib_File   *taglib_file,
                                     AudioTagsPage *page)
{
  TagLib_Tag *taglib_tag;

  /* Strings to be freed by taglib_tag_free_strings */
  gchar      *title;
  gchar      *artist;
  gchar      *album;
  gchar      *comment;
  gchar      *genre;

  guint       track;
  guint       year;

  g_return_if_fail (IS_AUDIO_TAGS_PAGE (page));
  g_return_if_fail (page->taglib_file == taglib_file);
  g_return_if_fail (taglib_file != NULL);

  /* Load tag information */
  taglib_tag = taglib_file_tag (taglib_file);

  if (G_LIKELY (taglib_tag != NULL))
    {
      /* Read values */
      track = taglib_tag_track (taglib_tag);
      title = taglib_tag_title (taglib_tag);
      artist = taglib_tag_artist (taglib_tag);
      album = taglib_tag_album (taglib_tag);
      comment = taglib_tag_comment (taglib_tag);
      genre = taglib_tag_genre (taglib_tag);
      year = taglib_tag_year (taglib_tag);

      if (track == 0)
        track = 1;

      if (year == 0)
        year = 2006;

      /* Set page properties in order to sync with display */
      g_object_set (G_OBJECT (page), 
                    "track", (gdouble)track, 
                    "year", (gdouble)year, 
                    "artist", artist, 
                    "title", title,
                    "album", album,
                    "comment", comment,
                    "genre", genre,
                    NULL);
      
      /* Free allocated strings */
      taglib_tag_free_strings ();
    }
}



static gboolean
audio_tags_page_activate (AudioTagsPage *page)
{
  TagLib_Tag *tag;
    
  g_return_val_if_fail (page != NULL || IS_AUDIO_TAGS_PAGE (page), FALSE);
  g_return_val_if_fail (page->file != NULL || THUNARX_IS_FILE_INFO (page->file), FALSE);
  g_return_val_if_fail (page->taglib_file != NULL, FALSE);

  /* Get tag information */
  tag = taglib_file_tag (page->taglib_file);

  if (G_LIKELY (tag != NULL))
    {
      /* Compare old to new values */
      if (taglib_tag_track (tag) != page->track ||
          taglib_tag_year (tag) != page->year ||
          g_utf8_collate (taglib_tag_artist (tag), page->artist) ||
          g_utf8_collate (taglib_tag_title (tag), page->title) ||
          g_utf8_collate (taglib_tag_album (tag), page->album) ||
          g_utf8_collate (taglib_tag_comment (tag), page->comment) ||
          g_utf8_collate (taglib_tag_genre (tag), page->genre))
        {
          /* Make page insensitive */
          gtk_widget_set_sensitive (GTK_WIDGET (page), FALSE);
          
          /* Store values */
          taglib_tag_set_track (tag, page->track);
          taglib_tag_set_year (tag, page->year);
          taglib_tag_set_title (tag, page->title);
          taglib_tag_set_artist (tag, page->artist);
          taglib_tag_set_album (tag, page->album);
          taglib_tag_set_comment (tag, page->comment);
          taglib_tag_set_genre (tag, page->genre);

          /* Save file */
          taglib_file_save (page->taglib_file);
        }

      /* Free tag strings */
      taglib_tag_free_strings ();
    }
  
  return FALSE;
}



static gboolean
audio_tags_page_load_tags (AudioTagsPage *page)
{
  TagLib_File *taglib_file;
  gchar       *uri;
  gchar       *filename;

  g_return_val_if_fail (page != NULL || IS_AUDIO_TAGS_PAGE (page), FALSE);
  g_return_val_if_fail (page->file != NULL || THUNARX_IS_FILE_INFO (page->file), FALSE);

  /* Determine filename */
  uri = thunarx_file_info_get_uri (page->file);
  filename = g_filename_from_uri (uri, NULL, NULL);

  /* Try to load the taglib file */
  taglib_file = taglib_file_new (filename);

  /* Set taglib file */
  if (G_LIKELY (taglib_file != NULL))
      audio_tags_page_set_taglib_file (page, taglib_file);

  /* Free strings */
  g_free (filename);
  g_free (uri);

  /* Reset timeout */
  page->changed_timeout = 0;

  return FALSE;
}
