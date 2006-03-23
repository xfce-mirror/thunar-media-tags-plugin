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

#include <thunar-sbr-tag-renamer.h>



static GType tag_renamer_scheme_type;



GType
thunar_sbr_tag_renamer_scheme_get_type (void)
{
  return tag_renamer_scheme_type;
}



void
thunar_sbr_register_tag_enum_types (ThunarxProviderPlugin *plugin)
{
  static const GEnumValue tag_renamer_scheme_values[] =
  {
    { THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_TITLE,            "THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_TITLE",            N_ ("Track - Title"), },
    { THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_ARTIST_TITLE,     "THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_ARTIST_TITLE",     N_ ("Track - Artist - Title"), },
    { THUNAR_SBR_TAG_RENAMER_SCHEME_TITLE,                  "THUNAR_SBR_TAG_RENAMER_SCHEME_TITLE",                  N_ ("Title"),  },
    { THUNAR_SBR_TAG_RENAMER_SCHEME_ARTIST_TITLE,           "THUNAR_SBR_TAG_RENAMER_SCHEME_ARTIST_TITLE",           N_ ("Artist - Title"), },
    { THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_TITLE,        "THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_TITLE",        N_ ("Track. Title"), },
    { THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_ARTIST_TITLE, "THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_ARTIST_TITLE", N_ ("Track. Artist - Title"), },
    { 0,                                                    NULL,                                                   NULL, },
  };

  tag_renamer_scheme_type = thunarx_provider_plugin_register_enum (plugin, "ThunarSbrTagRenamerScheme", tag_renamer_scheme_values);
}



/* Property identifiers */
enum
{
  PROP_0,
  PROP_NAMING_SCHEME,
  PROP_REPLACE_SPACES,
  PROP_LOWERCASE,
};



static void   thunar_sbr_tag_renamer_class_init   (ThunarSbrTagRenamerClass *klass);
static void   thunar_sbr_tag_renamer_init         (ThunarSbrTagRenamer      *tag_renamer);
static void   thunar_sbr_tag_renamer_finalize     (GObject                  *object);
static void   thunar_sbr_tag_renamer_get_property (GObject                  *object,
                                                   guint                     prop_id,
                                                   GValue                   *value,
                                                   GParamSpec               *pspec);
static void   thunar_sbr_tag_renamer_set_property (GObject                  *object,
                                                   guint                     prop_id,
                                                   const GValue             *value,
                                                   GParamSpec               *pspec);
static void   thunar_sbr_tag_renamer_realize      (GtkWidget                *widget);
static gchar *thunar_sbr_tag_renamer_process      (ThunarxRenamer           *renamer,
                                                   ThunarxFileInfo          *file,
                                                   const gchar              *text,
                                                   guint                     index);



struct _ThunarSbrTagRenamerClass
{
  ThunarxRenamerClass __parent__;
};

struct _ThunarSbrTagRenamer
{
  ThunarxRenamer __parent__;

  /* Widgets */
  GtkTooltips              *tooltips;

  /* Properties */
  ThunarSbrTagRenamerScheme scheme;
  gboolean                  replace_spaces;
  gboolean                  lowercase;
};



THUNARX_DEFINE_TYPE (ThunarSbrTagRenamer, thunar_sbr_tag_renamer, THUNARX_TYPE_RENAMER);



static void
thunar_sbr_tag_renamer_class_init (ThunarSbrTagRenamerClass *klass)
{
  ThunarxRenamerClass *thunarxrenamer_class;
  GtkWidgetClass      *gtkwidget_class;
  GObjectClass        *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_sbr_tag_renamer_finalize;
  gobject_class->get_property = thunar_sbr_tag_renamer_get_property;
  gobject_class->set_property = thunar_sbr_tag_renamer_set_property;
  
  gtkwidget_class = GTK_WIDGET_CLASS (klass);
  gtkwidget_class->realize = thunar_sbr_tag_renamer_realize;

  thunarxrenamer_class = THUNARX_RENAMER_CLASS (klass);
  thunarxrenamer_class->process = thunar_sbr_tag_renamer_process;

  /**
   * ThunarSbrTagRenamer:naming-scheme:
   *
   * Naming scheme used for renaming the selected files.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_NAMING_SCHEME,
                                   g_param_spec_enum ("naming-scheme",
                                                      "naming-scheme",
                                                      "naming-scheme",
                                                      THUNAR_SBR_TYPE_TAG_RENAMER_SCHEME,
                                                      THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_TITLE,
                                                      G_PARAM_READWRITE));

  /**
   * ThunarSbrTagRenamer:replace-spaces:
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
   * ThunarSbrTagRenamer:lowercase:
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
}



static void 
thunar_sbr_tag_renamer_init (ThunarSbrTagRenamer *tag_renamer)
{
  AtkRelationSet *relations;
  AtkRelation    *relation;
  AtkObject      *object;
  GtkWidget      *vbox;
  GtkWidget      *hbox;
  GtkWidget      *label;
  GtkWidget      *combo;
  GtkWidget      *button;
  GEnumClass     *klass;
  gint            n;

  /* Allocate shared tooltips */
  tag_renamer->tooltips = gtk_tooltips_new ();
  exo_gtk_object_ref_sink (GTK_OBJECT (tag_renamer->tooltips));

  vbox = gtk_vbox_new (FALSE, 12);
  gtk_box_pack_start (GTK_BOX (tag_renamer), vbox, FALSE, FALSE, 0);
  gtk_widget_show (vbox);
  
  hbox = gtk_hbox_new (FALSE, 12);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  /* Naming scheme label */
  label = gtk_label_new_with_mnemonic (_("Naming _scheme:"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  /* Naming scheme combo box */
  combo = gtk_combo_box_new_text ();
  klass = g_type_class_ref (THUNAR_SBR_TYPE_TAG_RENAMER_SCHEME);
  for (n = 0; n < klass->n_values; ++n)
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _(klass->values[n].value_nick));
  exo_mutual_binding_new (G_OBJECT (tag_renamer), "naming-scheme", G_OBJECT (combo), "active");
  gtk_box_pack_start (GTK_BOX (hbox), combo, FALSE, FALSE, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), combo);
  g_type_class_unref (klass);
  gtk_widget_show (combo);

  /* Set Atk label relation for the artist entry */
  object = gtk_widget_get_accessible (combo);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  hbox = gtk_hbox_new (FALSE, 12);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  button = gtk_check_button_new_with_mnemonic (_("_Replace spaces with underscores"));
  exo_mutual_binding_new (G_OBJECT (button), "active", G_OBJECT (tag_renamer), "replace-spaces");
  gtk_tooltips_set_tip (tag_renamer->tooltips, button, _("Activating this option will replace all spaces in the target filename "
        "with underscores."), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_check_button_new_with_mnemonic (_("Convert to _lowercase"));
  exo_mutual_binding_new (G_OBJECT (button), "active", G_OBJECT (tag_renamer), "lowercase");
  gtk_tooltips_set_tip (tag_renamer->tooltips, button, _("If you activate this, the resulting filename will not contain any "
        "uppercase letters."), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);
}



static void
thunar_sbr_tag_renamer_finalize (GObject *object)
{
  /* ThunarSbrTagRenamer *tag_renamer = THUNAR_SBR_TAG_RENAMER (object); */

  (*G_OBJECT_CLASS (thunar_sbr_tag_renamer_parent_class)->finalize) (object);
}



static void
thunar_sbr_tag_renamer_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  ThunarSbrTagRenamer *tag_renamer = THUNAR_SBR_TAG_RENAMER (object);

  switch (prop_id)
    {
    case PROP_NAMING_SCHEME:
      g_value_set_enum (value, thunar_sbr_tag_renamer_get_scheme (tag_renamer));
      break;

    case PROP_REPLACE_SPACES:
      g_value_set_boolean (value, thunar_sbr_tag_renamer_get_replace_spaces (tag_renamer));
      break;

    case PROP_LOWERCASE:
      g_value_set_boolean (value, thunar_sbr_tag_renamer_get_lowercase (tag_renamer));
      break;
      
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }    
}



static void
thunar_sbr_tag_renamer_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  ThunarSbrTagRenamer *tag_renamer = THUNAR_SBR_TAG_RENAMER (object);

  switch (prop_id)
    {
    case PROP_NAMING_SCHEME:
      thunar_sbr_tag_renamer_set_scheme (tag_renamer, g_value_get_enum (value));
      break;

    case PROP_REPLACE_SPACES:
      thunar_sbr_tag_renamer_set_replace_spaces (tag_renamer, g_value_get_boolean (value));
      break;

    case PROP_LOWERCASE:
      thunar_sbr_tag_renamer_set_lowercase (tag_renamer, g_value_get_boolean (value));
      break;
      
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_sbr_tag_renamer_realize (GtkWidget *widget)
{
  /* Realize the widget */
  (*GTK_WIDGET_CLASS (thunar_sbr_tag_renamer_parent_class)->realize) (widget);
}



static gchar*
thunar_sbr_tag_renamer_process (ThunarxRenamer  *renamer,
                                ThunarxFileInfo *file,
                                const gchar     *text,
                                guint            index)
{
  ThunarSbrTagRenamer *tag_renamer = THUNAR_SBR_TAG_RENAMER (renamer);

  TagLib_File                  *taglib_file;
  TagLib_Tag                   *taglib_tag;
  const TagLib_AudioProperties *taglib_properties;

  gchar *result = NULL;
  gchar *filename;
  gchar *artist;
  gchar *title;
  guint  track_num;
  gchar *track;
  
  /* Strip of the file:// from the URI */
  filename = g_filename_from_uri (thunarx_file_info_get_uri (file), NULL, NULL);

  /* Return if URI could not be resolved */
  if (G_UNLIKELY (filename == NULL))
    return g_strdup (text);

  /* Load taglib file */
  taglib_file = taglib_file_new (filename);

  /* Don't change anything if taglib file could not be allocated */
  if (G_UNLIKELY (taglib_file == NULL))
    return g_strdup (text);

  /* Load tag info */
  taglib_tag = taglib_file_tag (taglib_file);

  /* Abort if tag info could not be loaded */
  if (G_UNLIKELY (taglib_tag == NULL))
    return g_strdup (text);

  /* Load tag properties */
  taglib_properties = taglib_file_audioproperties (taglib_file);

  /* Get tag values */
  artist = taglib_tag_artist (taglib_tag);
  title = taglib_tag_title (taglib_tag);
  track_num = taglib_tag_track (taglib_tag);

  if (track_num == 0)
    track = g_strdup ("00");
  else
    track = g_strdup_printf ("%02d", taglib_tag_track (taglib_tag));

  switch (thunar_sbr_tag_renamer_get_scheme (tag_renamer))
    {
    case THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_TITLE:
      result = g_strconcat (track, " - ", title, NULL);
      break;

    case THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_ARTIST_TITLE:
      result = g_strconcat (track, " - ", artist, " - ", title, NULL);
      break;

    case THUNAR_SBR_TAG_RENAMER_SCHEME_TITLE:
      result = g_strconcat (title, NULL);
      break;

    case THUNAR_SBR_TAG_RENAMER_SCHEME_ARTIST_TITLE:
      result = g_strconcat (artist, " - ", title, NULL);
      break;

    case THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_TITLE:
      result = g_strconcat (track, ". ", title, NULL);
      break;

    case THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_DOT_ARTIST_TITLE:
      result = g_strconcat (track, ". ", artist, " - ", title, NULL);
      break;
    
    default:
      result = g_strdup (text);
      break;
    }

  /* Replace spaces if requested */
  if (thunar_sbr_tag_renamer_get_replace_spaces (tag_renamer))
    result = g_strdelimit (result, " ", '_');
  
  /* Convert to lowercase if requested */
  if (thunar_sbr_tag_renamer_get_lowercase (tag_renamer))
  {
    gchar *tmp = result;
    result = g_utf8_strdown (result, -1);
    g_free (tmp);
  }
  
  /* Free strings */
  g_free (filename);
  g_free (track);

  /* Free tag info strings */
  taglib_tag_free_strings ();

  /* Destroy the taglib file */
  taglib_file_free (taglib_file);

  return result;
}



/**
 * thunar_sbr_tag_renamer_new:
 *
 * Allocates a new #ThunarSbrTagRenamer instance.
 *
 * Return value: the newly allocated #ThunarSbrTagRenamer.
 **/
ThunarSbrTagRenamer*
thunar_sbr_tag_renamer_new (void)
{
  return g_object_new (THUNAR_SBR_TYPE_TAG_RENAMER,
                       "name", _("ID3 and Vorbis Tags"),
                       NULL);
}



/**
 * thunar_sbr_tag_renamer_get_scheme:
 * @tag_renamer : a #ThunarSbrTagRenamer.
 *
 * Returns the current #ThunarSbrTagRenamerScheme
 * for the @tag_renamer.
 *
 * Return value: the current naming scheme of @tag_renamer.
 **/
ThunarSbrTagRenamerScheme
thunar_sbr_tag_renamer_get_scheme (ThunarSbrTagRenamer *tag_renamer)
{
  g_return_val_if_fail (THUNAR_SBR_IS_TAG_RENAMER (tag_renamer), THUNAR_SBR_TAG_RENAMER_SCHEME_TRACK_TITLE);
  return tag_renamer->scheme;
}



/**
 * thunar_sbr_tag_renamer_set_scheme:
 * @tag_renamer : a #ThunarSbrTagRenamer.
 * @scheme      : the new #ThunarSbrTagRenamerScheme.
 *
 * Sets the #ThunarSbrTagRenamerScheme of the @tag_renamer
 * to the specified @scheme.
 **/
void
thunar_sbr_tag_renamer_set_scheme (ThunarSbrTagRenamer      *tag_renamer,
                                   ThunarSbrTagRenamerScheme scheme)
{
  g_return_if_fail (THUNAR_SBR_IS_TAG_RENAMER (tag_renamer));

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
 * thunar_sbr_tag_renamer_get_replace_spaces:
 * @tag_renamer : a #ThunarSbrTagRenamer.
 *
 * Returns whether spaces are going to be replaced with underscores or not.
 *
 * Return value: whether spaces are going to be replaced or not.
 **/
gboolean
thunar_sbr_tag_renamer_get_replace_spaces (ThunarSbrTagRenamer *tag_renamer)
{
  g_return_val_if_fail (THUNAR_SBR_IS_TAG_RENAMER (tag_renamer), FALSE);
  return tag_renamer->replace_spaces;
}



/**
 * thunar_sbr_tag_renamer_set_replace_spaces:
 * @tag_renamer : a #ThunarSbrTagRenamer.
 * @replace     : a boolean flag.
 *
 * Sets whether spaces are going to be replaced with underscores or not.
 **/
void
thunar_sbr_tag_renamer_set_replace_spaces (ThunarSbrTagRenamer *tag_renamer,
                                           gboolean             replace)
{
  g_return_if_fail (THUNAR_SBR_IS_TAG_RENAMER (tag_renamer));

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
 * thunar_sbr_tag_renamer_get_lowercase:
 * @tag_renamer : a #ThunarSbrTagRenamer.
 *
 * Returns whether the resulting filename will be completely lowercase.
 *
 * Return value: whether all letters will be lowercase.
 **/
gboolean
thunar_sbr_tag_renamer_get_lowercase (ThunarSbrTagRenamer *tag_renamer)
{
  g_return_val_if_fail (THUNAR_SBR_IS_TAG_RENAMER (tag_renamer), FALSE);
  return tag_renamer->lowercase;
}



/**
 * thunar_sbr_tag_renamer_set_lowercase:
 * @tag_renamer : a #ThunarSbrTagRenamer.
 * @lowercase     : a boolean flag.
 *
 * Sets whether all letters are going to be converted to lowercase.
 **/
void
thunar_sbr_tag_renamer_set_lowercase (ThunarSbrTagRenamer *tag_renamer,
                                      gboolean             lowercase)
{
  g_return_if_fail (THUNAR_SBR_IS_TAG_RENAMER (tag_renamer));

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
