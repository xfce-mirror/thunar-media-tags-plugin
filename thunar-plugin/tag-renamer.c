/* $Id: tag-renamer.c 1205 2006-03-23 12:43:58Z jpohlmann $ */
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

#include <tag-renamer.h>



static GType tag_renamer_scheme_type;



GType
tag_renamer_scheme_get_type (void)
{
  return tag_renamer_scheme_type;
}



void
register_tag_enum_types (ThunarxProviderPlugin *plugin)
{
  static const GEnumValue tag_renamer_scheme_values[] =
  {
    { TAG_RENAMER_SCHEME_TITLE,                  "TAG_RENAMER_SCHEME_TITLE",                  N_ ("Title"),  },
    { TAG_RENAMER_SCHEME_ARTIST_TITLE,           "TAG_RENAMER_SCHEME_ARTIST_TITLE",           N_ ("Artist - Title"), },
    { TAG_RENAMER_SCHEME_TRACK_TITLE,            "TAG_RENAMER_SCHEME_TRACK_TITLE",            N_ ("Track - Title"), },
    { TAG_RENAMER_SCHEME_TRACK_ARTIST_TITLE,     "TAG_RENAMER_SCHEME_TRACK_ARTIST_TITLE",     N_ ("Track - Artist - Title"), },
    { TAG_RENAMER_SCHEME_TRACK_DOT_TITLE,        "TAG_RENAMER_SCHEME_TRACK_DOT_TITLE",        N_ ("Track. Title"), },
    { TAG_RENAMER_SCHEME_TRACK_DOT_ARTIST_TITLE, "TAG_RENAMER_SCHEME_TRACK_DOT_ARTIST_TITLE", N_ ("Track. Artist - Title"), },
    { TAG_RENAMER_SCHEME_ARTIST_TRACK_TITLE,     "TAG_RENAMER_SCHEME_ARTIST_TRACK_TITLE",     N_ ("Artist - Track - Title"), },
    { 0,                                         NULL,                                        NULL, },
  };

  tag_renamer_scheme_type = thunarx_provider_plugin_register_enum (plugin, "TagRenamerScheme", tag_renamer_scheme_values);
}



/* Property identifiers */
enum
{
  PROP_0,
  PROP_NAMING_SCHEME,
  PROP_REPLACE_SPACES,
  PROP_LOWERCASE,
  PROP_ARTIST,
  PROP_TITLE,
};



static void   tag_renamer_class_init   (TagRenamerClass *klass);
static void   tag_renamer_init         (TagRenamer      *tag_renamer);
static void   tag_renamer_finalize     (GObject         *object);
static void   tag_renamer_get_property (GObject         *object,
                                        guint            prop_id,
                                        GValue          *value,
                                        GParamSpec      *pspec);
static void   tag_renamer_set_property (GObject         *object,
                                        guint            prop_id,
                                        const GValue    *value,
                                        GParamSpec      *pspec);
static void   tag_renamer_realize      (GtkWidget       *widget);
static gchar *tag_renamer_process      (ThunarxRenamer  *renamer,
                                        ThunarxFileInfo *file,
                                        const gchar     *text,
                                        guint            index);



struct _TagRenamerClass
{
  ThunarxRenamerClass __parent__;
};

struct _TagRenamer
{
  ThunarxRenamer __parent__;

  /* Widgets */
  GtkTooltips     *tooltips;

  /* Properties */
  TagRenamerScheme scheme;
  gboolean         replace_spaces;
  gboolean         lowercase;
  gchar           *artist;
  gchar           *title;
};



THUNARX_DEFINE_TYPE (TagRenamer, tag_renamer, THUNARX_TYPE_RENAMER);



static void
tag_renamer_class_init (TagRenamerClass *klass)
{
  ThunarxRenamerClass *thunarxrenamer_class;
  GtkWidgetClass      *gtkwidget_class;
  GObjectClass        *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = tag_renamer_finalize;
  gobject_class->get_property = tag_renamer_get_property;
  gobject_class->set_property = tag_renamer_set_property;
  
  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->realize = tag_renamer_realize;

  thunarxrenamer_class = THUNARX_RENAMER_CLASS (klass);
  thunarxrenamer_class->process = tag_renamer_process;

  /**
   * TagRenamer:naming-scheme:
   *
   * Naming scheme used for renaming the selected files.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_NAMING_SCHEME,
                                   g_param_spec_enum ("naming-scheme",
                                                      "naming-scheme",
                                                      "naming-scheme",
                                                      TYPE_TAG_RENAMER_SCHEME,
                                                      TAG_RENAMER_SCHEME_TRACK_TITLE,
                                                      G_PARAM_READWRITE));

  /**
   * TagRenamer:replace-spaces:
   *
   * Whether spaces should be replaced with _ or not
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_REPLACE_SPACES,
                                   g_param_spec_boolean ("replace-spaces",
                                                         "replace-spaces",
                                                         "replace-spaces",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

  /**
   * TagRenamer:lowercase:
   *
   * Whether all chars will be converted to lowercase.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_LOWERCASE,
                                   g_param_spec_boolean ("lowercase",
                                                         "lowercase",
                                                         "lowercase",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

  /**
   * TagRenamer:artist:
   *
   * The artist name to use if a file is missing an artist tag.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ARTIST,
                                   g_param_spec_string ("artist",
                                                        "artist",
                                                        "artist",
                                                        _("Unknown Artist"),
                                                        G_PARAM_READWRITE));

  /**
   * TagRenamer:title:
   *
   * The song title to use if a file is missing a proper title tag.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TITLE,
                                   g_param_spec_string ("title",
                                                        "title",
                                                        "title",
                                                        _("Unknown Title"),
                                                        G_PARAM_READWRITE));
}



static void 
tag_renamer_init (TagRenamer *tag_renamer)
{
  AtkRelationSet *relations;
  AtkRelation    *relation;
  AtkObject      *object;
  GtkWidget      *table;
  GtkWidget      *label;
  GtkWidget      *combo;
  GtkWidget      *entry;
  GtkWidget      *button;
  GEnumClass     *klass;
  gint            n;

  /* Allocate shared tooltips */
  tag_renamer->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (tag_renamer->tooltips));

  table = gtk_table_new (2, 3, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_box_pack_start (GTK_BOX (tag_renamer), table, FALSE, FALSE, 0);
  gtk_widget_show (table);

  /* Naming scheme label */
  label = gtk_label_new_with_mnemonic (_("Naming _scheme:"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.05f,  0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  /* Naming scheme combo box */
  combo = gtk_combo_box_new_text ();
  klass = g_type_class_ref (TYPE_TAG_RENAMER_SCHEME);
  for (n = 0; n < klass->n_values; ++n)
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _(klass->values[n].value_nick));
  exo_mutual_binding_new (G_OBJECT (tag_renamer), "naming-scheme", G_OBJECT (combo), "active");
  gtk_table_attach (GTK_TABLE (table), combo, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), combo);
  g_type_class_unref (klass);
  gtk_widget_show (combo);

  /* Set Atk label relation for the combo box */
  object = gtk_widget_get_accessible (combo);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  button = gtk_check_button_new_with_mnemonic (_("Use _underscores"));
  exo_mutual_binding_new (G_OBJECT (button), "active", G_OBJECT (tag_renamer), "replace-spaces");
  gtk_tooltips_set_tip (tag_renamer->tooltips, button, _("Activating this option will replace all spaces in the target filename "
        "with underscores."), NULL);
  gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1, GTK_FILL, 0, 0, 0);
  gtk_widget_show (button);

  button = gtk_check_button_new_with_mnemonic (_("Use _lowercase"));
  exo_mutual_binding_new (G_OBJECT (button), "active", G_OBJECT (tag_renamer), "lowercase");
  gtk_tooltips_set_tip (tag_renamer->tooltips, button, _("If you activate this, the resulting filename will not contain any "
        "uppercase letters."), NULL);
  gtk_table_attach (GTK_TABLE (table), button, 2, 3, 1, 2, GTK_FILL, 0, 0, 0);
  gtk_widget_show (button);

  label = gtk_label_new_with_mnemonic (_("Default Art_ist:"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.5f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0);
  gtk_widget_show (label);

  entry = gtk_entry_new ();
  gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
  exo_mutual_binding_new (G_OBJECT (entry), "text", G_OBJECT (tag_renamer), "artist");
  gtk_tooltips_set_tip (tag_renamer->tooltips, entry, _("Text entered in this field is going to replace missing artist tags."), NULL);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
  gtk_widget_show (entry);

  /* Set Atk label relation for this entry */
  object = gtk_widget_get_accessible (entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  /* Set initial values */
  tag_renamer->artist = g_strdup ("");
  tag_renamer->title = g_strdup ("");
  tag_renamer_set_artist (tag_renamer, _("Unknown Artist"));
  tag_renamer_set_title (tag_renamer, _("Unknown Title"));
}



static void
tag_renamer_finalize (GObject *object)
{
  TagRenamer *tag_renamer = TAG_RENAMER (object);

  /* release the tooltips */
  g_object_unref (G_OBJECT (tag_renamer->tooltips));

  /* Free strings */
  g_free (tag_renamer->artist);
  g_free (tag_renamer->title);

  (*G_OBJECT_CLASS (tag_renamer_parent_class)->finalize) (object);
}



static void
tag_renamer_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  TagRenamer *tag_renamer = TAG_RENAMER (object);

  switch (prop_id)
    {
    case PROP_NAMING_SCHEME:
      g_value_set_enum (value, tag_renamer_get_scheme (tag_renamer));
      break;

    case PROP_REPLACE_SPACES:
      g_value_set_boolean (value, tag_renamer_get_replace_spaces (tag_renamer));
      break;

    case PROP_LOWERCASE:
      g_value_set_boolean (value, tag_renamer_get_lowercase (tag_renamer));
      break;

    case PROP_ARTIST:
      g_value_set_string (value, tag_renamer_get_artist (tag_renamer));
      break;

    case PROP_TITLE:
      g_value_set_string (value, tag_renamer_get_title (tag_renamer));
      break;
      
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }    
}



static void
tag_renamer_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  TagRenamer *tag_renamer = TAG_RENAMER (object);

  switch (prop_id)
    {
    case PROP_NAMING_SCHEME:
      tag_renamer_set_scheme (tag_renamer, g_value_get_enum (value));
      break;

    case PROP_REPLACE_SPACES:
      tag_renamer_set_replace_spaces (tag_renamer, g_value_get_boolean (value));
      break;

    case PROP_LOWERCASE:
      tag_renamer_set_lowercase (tag_renamer, g_value_get_boolean (value));
      break;

    case PROP_ARTIST:
      tag_renamer_set_artist (tag_renamer, g_value_get_string (value));
      break;

    case PROP_TITLE:
      tag_renamer_set_title (tag_renamer, g_value_get_string (value));
      break;
      
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
tag_renamer_realize (GtkWidget *widget)
{
  /* Realize the widget */
  (*GTK_WIDGET_CLASS (tag_renamer_parent_class)->realize) (widget);
}



static gchar*
tag_renamer_process (ThunarxRenamer  *renamer,
                     ThunarxFileInfo *file,
                     const gchar     *text,
                     guint            index)
{
  TagRenamer *tag_renamer = TAG_RENAMER (renamer);

  TagLib_File                  *taglib_file;
  TagLib_Tag                   *taglib_tag;
  const TagLib_AudioProperties *taglib_properties;

  gchar *result = NULL;
  gchar *filename;
  gchar *artist;
  gchar *title;
  guint  track_num;
  gchar *track;
  gchar *uri;
  
  /* Strip of the file:// from the URI */
  uri = thunarx_file_info_get_uri (file);
  filename = g_filename_from_uri (uri, NULL, NULL);
  g_free (uri);

  /* Return if URI could not be resolved */
  if (G_UNLIKELY (filename == NULL))
    return g_strdup (text);

  /* Load taglib file */
  taglib_file = taglib_file_new (filename);

  /* Don't change anything if taglib file could not be allocated */
  if (G_UNLIKELY (taglib_file == NULL))
    {
      g_free (filename);
      return g_strdup (text);
    }

  /* Load tag info */
  taglib_tag = taglib_file_tag (taglib_file);

  /* Abort if tag info could not be loaded */
  if (G_UNLIKELY (taglib_tag == NULL))
    {
      taglib_file_free (taglib_file);
      g_free (filename);
      return g_strdup (text);
    }

  /* Load tag properties */
  taglib_properties = taglib_file_audioproperties (taglib_file);

  /* Get tag values */
  artist = g_strdup (taglib_tag_artist (taglib_tag));
  title = g_strdup (taglib_tag_title (taglib_tag));
  track_num = taglib_tag_track (taglib_tag);

  /* Strip additional whitespace from the tags */
  g_strstrip (artist);
  g_strstrip (title);

  /* Replace missing artist tag */
  if (G_UNLIKELY (g_utf8_strlen (artist, -1) == 0))
    artist = g_strdup (tag_renamer_get_artist (tag_renamer));

  /* Replace missing song title tag */
  if (G_UNLIKELY (g_utf8_strlen (title, -1) == 0))
    title = g_strdup (tag_renamer_get_title (tag_renamer));

  if (track_num == 0)
    track = g_strdup ("00");
  else
    track = g_strdup_printf ("%02d", taglib_tag_track (taglib_tag));

  /* Replace special chars with underscores */
  artist = g_strdelimit (artist, G_DIR_SEPARATOR_S, '_');
  title = g_strdelimit (title, G_DIR_SEPARATOR_S, '_');
  track = g_strdelimit (track, G_DIR_SEPARATOR_S, '_');

  switch (tag_renamer_get_scheme (tag_renamer))
    {
    case TAG_RENAMER_SCHEME_TRACK_DOT_TITLE:
      result = g_strconcat (track, ". ", title, NULL);
      break;

    case TAG_RENAMER_SCHEME_TRACK_DOT_ARTIST_TITLE:
      result = g_strconcat (track, ". ", artist, " - ", title, NULL);
      break;

    case TAG_RENAMER_SCHEME_TRACK_TITLE:
      result = g_strconcat (track, " - ", title, NULL);
      break;

    case TAG_RENAMER_SCHEME_TRACK_ARTIST_TITLE:
      result = g_strconcat (track, " - ", artist, " - ", title, NULL);
      break;

    case TAG_RENAMER_SCHEME_TITLE:
      result = g_strconcat (title, NULL);
      break;

    case TAG_RENAMER_SCHEME_ARTIST_TITLE:
      result = g_strconcat (artist, " - ", title, NULL);
      break;

    case TAG_RENAMER_SCHEME_ARTIST_TRACK_TITLE:
      result = g_strconcat (artist, " - ", track, " - ", title, NULL);
      break;
    
    default:
      result = g_strdup (text);
      break;
    }

  /* Replace spaces if requested */
  if (tag_renamer_get_replace_spaces (tag_renamer))
    result = g_strdelimit (result, " ", '_');
  
  /* Convert to lowercase if requested */
  if (tag_renamer_get_lowercase (tag_renamer))
  {
    gchar *tmp = result;
    result = g_utf8_strdown (result, -1);
    g_free (tmp);
  }
  
  /* Free strings */
  g_free (filename);
  g_free (artist);
  g_free (title);
  g_free (track);

  /* Free tag info strings */
  taglib_tag_free_strings ();

  /* Destroy the taglib file */
  taglib_file_free (taglib_file);

  return result;
}



/**
 * tag_renamer_new:
 *
 * Allocates a new #TagRenamer instance.
 *
 * Return value: the newly allocated #TagRenamer.
 **/
TagRenamer*
tag_renamer_new (void)
{
  return g_object_new (TYPE_TAG_RENAMER,
                       "name", _("ID3 and Vorbis Tags"),
                       NULL);
}



/**
 * tag_renamer_get_scheme:
 * @tag_renamer : a #TagRenamer.
 *
 * Returns the current #TagRenamerScheme
 * for the @tag_renamer.
 *
 * Return value: the current naming scheme of @tag_renamer.
 **/
TagRenamerScheme
tag_renamer_get_scheme (TagRenamer *tag_renamer)
{
  g_return_val_if_fail (IS_TAG_RENAMER (tag_renamer), TAG_RENAMER_SCHEME_TRACK_TITLE);
  return tag_renamer->scheme;
}



/**
 * tag_renamer_set_scheme:
 * @tag_renamer : a #TagRenamer.
 * @scheme      : the new #TagRenamerScheme.
 *
 * Sets the #TagRenamerScheme of the @tag_renamer
 * to the specified @scheme.
 **/
void
tag_renamer_set_scheme (TagRenamer      *tag_renamer,
                        TagRenamerScheme scheme)
{
  g_return_if_fail (IS_TAG_RENAMER (tag_renamer));

  /* Check if we already use that scheme */
  if (G_UNLIKELY (tag_renamer->scheme == scheme))
    return;

  /* Apply the new scheme */
  tag_renamer->scheme = scheme;

  /* Notify listeners */
  g_object_notify (G_OBJECT (tag_renamer), "naming-scheme");

  /* Emit the "changed" signal */
  thunarx_renamer_changed (THUNARX_RENAMER (tag_renamer));
}



/**
 * tag_renamer_get_replace_spaces:
 * @tag_renamer : a #TagRenamer.
 *
 * Returns whether spaces are going to be replaced with underscores or not.
 *
 * Return value: whether spaces are going to be replaced or not.
 **/
gboolean
tag_renamer_get_replace_spaces (TagRenamer *tag_renamer)
{
  g_return_val_if_fail (IS_TAG_RENAMER (tag_renamer), FALSE);
  return tag_renamer->replace_spaces;
}



/**
 * tag_renamer_set_replace_spaces:
 * @tag_renamer : a #TagRenamer.
 * @replace     : a boolean flag.
 *
 * Sets whether spaces are going to be replaced with underscores or not.
 **/
void
tag_renamer_set_replace_spaces (TagRenamer *tag_renamer,
                                gboolean    replace)
{
  g_return_if_fail (IS_TAG_RENAMER (tag_renamer));

  /* Abort if flag is already set */
  if (G_UNLIKELY (tag_renamer->replace_spaces == replace))
    return;

  /* Apply the new setting */
  tag_renamer->replace_spaces = replace;

  /* Notify listeners */
  g_object_notify (G_OBJECT (tag_renamer), "replace-spaces");

  /* Emit the "changed" signal */
  thunarx_renamer_changed (THUNARX_RENAMER (tag_renamer));
}



/**
 * tag_renamer_get_lowercase:
 * @tag_renamer : a #TagRenamer.
 *
 * Returns whether the resulting filename will be completely lowercase.
 *
 * Return value: whether all letters will be lowercase.
 **/
gboolean
tag_renamer_get_lowercase (TagRenamer *tag_renamer)
{
  g_return_val_if_fail (IS_TAG_RENAMER (tag_renamer), FALSE);
  return tag_renamer->lowercase;
}



/**
 * tag_renamer_set_lowercase:
 * @tag_renamer : a #TagRenamer.
 * @lowercase     : a boolean flag.
 *
 * Sets whether all letters are going to be converted to lowercase.
 **/
void
tag_renamer_set_lowercase (TagRenamer *tag_renamer,
                           gboolean    lowercase)
{
  g_return_if_fail (IS_TAG_RENAMER (tag_renamer));

  /* Abort if flag is already set */
  if (G_UNLIKELY (tag_renamer->lowercase == lowercase))
    return;

  /* Apply the new setting */
  tag_renamer->lowercase = lowercase;

  /* Notify listeners */
  g_object_notify (G_OBJECT (tag_renamer), "lowercase");

  /* Emit the "changed" signal */
  thunarx_renamer_changed (THUNARX_RENAMER (tag_renamer));
}



/**
 * tag_renamer_get_artist:
 * @tag_renamer : a #TagRenamer.
 *
 * Returns the artist name to use if the artist tag is missing.
 *
 * Return value: default artist name to use if a file lacks of an artist tag.
 **/
const gchar*
tag_renamer_get_artist (TagRenamer *tag_renamer)
{
  g_return_val_if_fail (IS_TAG_RENAMER (tag_renamer), _("Unknown Artist"));
  return tag_renamer->artist;
}



/**
 * tag_renamer_set_artist:
 * @tag_renamer : a #TagRenamer.
 * @artist      : a nul-terminated string.
 *
 * Sets the artist name to use if a file is missing an artist tag.
 **/
void
tag_renamer_set_artist (TagRenamer  *tag_renamer,
                        const gchar *artist)
{
  g_return_if_fail (IS_TAG_RENAMER (tag_renamer));

  /* Abort if flag is already set */
  if (G_UNLIKELY (tag_renamer->artist != NULL))
    if (G_UNLIKELY (g_utf8_collate (tag_renamer->artist, artist) == 0))
      return;

  /* Free old name */
  if (G_LIKELY (tag_renamer->artist != NULL))
    g_free (tag_renamer->artist);

  /* Apply the new setting */
  tag_renamer->artist = g_strdup (artist);

  /* Notify listeners */
  g_object_notify (G_OBJECT (tag_renamer), "artist");

  /* Emit the "changed" signal */
  thunarx_renamer_changed (THUNARX_RENAMER (tag_renamer));
}



/**
 * tag_renamer_get_title:
 * @tag_renamer : a #TagRenamer.
 *
 * Returns the song title to use if the title tag is missing.
 *
 * Return value: song title to use if a file lacks of a title tag.
 **/
const gchar*
tag_renamer_get_title (TagRenamer *tag_renamer)
{
  g_return_val_if_fail (IS_TAG_RENAMER (tag_renamer), _("Unknown Title"));
  return tag_renamer->title;
}



/**
 * tag_renamer_set_title:
 * @tag_renamer : a #TagRenamer.
 * @title      : a nul-terminated string.
 *
 * Sets the song title to use if a file is missing a title tag.
 **/
void
tag_renamer_set_title (TagRenamer  *tag_renamer,
                       const gchar *title)
{
  g_return_if_fail (IS_TAG_RENAMER (tag_renamer));

  /* Abort if flag is already set */
  if (G_UNLIKELY (g_utf8_collate (tag_renamer->title, title) == 0))
    return;

  /* Free old value */
  if (G_UNLIKELY (tag_renamer->title != NULL))
    if (G_LIKELY (tag_renamer->title != NULL))
      g_free (tag_renamer->title);

  /* Apply the new setting */
  tag_renamer->title = g_strdup (title);

  /* Notify listeners */
  g_object_notify (G_OBJECT (tag_renamer), "title");

  /* Emit the "changed" signal */
  thunarx_renamer_changed (THUNARX_RENAMER (tag_renamer));
}
