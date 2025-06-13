/* $Id$ */
/*-
 * Copyright (c) 2006 Jannis Pohlmann <jannis@xfce.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA.
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

#include <libxfce4util/libxfce4util.h>

#include <tag-renamer.h>
#include <audio-tags-page.h>
#include <media-tags-provider.h>



static GType tag_renamer_format_type;



GType
tag_renamer_format_get_type (void)
{
  return tag_renamer_format_type;
}



void
tag_renamer_register_enum_types (ThunarxProviderPlugin *plugin)
{
  static const GEnumValue tag_renamer_format_values[] =
  {
    { TAG_RENAMER_FORMAT_TITLE,                  "TAG_RENAMER_FORMAT_TITLE",                  N_ ("Title"),  },
    { TAG_RENAMER_FORMAT_ARTIST_TITLE,           "TAG_RENAMER_FORMAT_ARTIST_TITLE",           N_ ("Artist - Title"), },
    { TAG_RENAMER_FORMAT_TRACK_TITLE,            "TAG_RENAMER_FORMAT_TRACK_TITLE",            N_ ("Track - Title"), },
    { TAG_RENAMER_FORMAT_TRACK_ARTIST_TITLE,     "TAG_RENAMER_FORMAT_TRACK_ARTIST_TITLE",     N_ ("Track - Artist - Title"), },
    { TAG_RENAMER_FORMAT_TRACK_DOT_TITLE,        "TAG_RENAMER_FORMAT_TRACK_DOT_TITLE",        N_ ("Track. Title"), },
    { TAG_RENAMER_FORMAT_TRACK_DOT_ARTIST_TITLE, "TAG_RENAMER_FORMAT_TRACK_DOT_ARTIST_TITLE", N_ ("Track. Artist - Title"), },
    { TAG_RENAMER_FORMAT_ARTIST_TRACK_TITLE,     "TAG_RENAMER_FORMAT_ARTIST_TRACK_TITLE",     N_ ("Artist - Track - Title"), },
    { TAG_RENAMER_FORMAT_SEPARATOR,              "TAG_RENAMER_FORMAT_SEPARATOR",              " ", },
    { TAG_RENAMER_FORMAT_CUSTOM,                 "TAG_RENAMER_FORMAT_CUSTOM",                 N_ ("Custom"), },
    { 0,                                         NULL,                                        NULL, },
  };

  tag_renamer_format_type = thunarx_provider_plugin_register_enum (plugin, "TagRenamerFormat", tag_renamer_format_values);
}



enum
{
  COLUMN_TEXT,
  COLUMN_NUMBER,
  N_COLUMNS
};



/* Property identifiers */
enum
{
  PROP_0,
  PROP_FORMAT,
  PROP_TEXT,
  PROP_REPLACE_SPACES,
  PROP_LOWERCASE,
};



static void      tag_renamer_finalize              (GObject         *object);
static void      tag_renamer_get_property          (GObject         *object,
                                                    guint            prop_id,
                                                    GValue          *value,
                                                    GParamSpec      *pspec);
static void      tag_renamer_set_property          (GObject         *object,
                                                    guint            prop_id,
                                                    const GValue    *value,
                                                    GParamSpec      *pspec);
static gchar    *tag_renamer_process               (ThunarxRenamer  *renamer,
                                                    ThunarxFileInfo *file,
                                                    const gchar     *text,
                                                    guint            index);
static GList    *tag_renamer_get_menu_items        (ThunarxRenamer  *renamer,
                                                    GtkWindow       *window,
                                                    GList           *files);
static void      tag_renamer_help_clicked          (GtkWidget       *button);
static gboolean  tag_renamer_combo_separator_func  (GtkTreeModel    *model,
                                                    GtkTreeIter     *iter,
                                                    gpointer         user_data);
static void      tag_renamer_combo_changed         (GtkComboBox     *combo_box,
                                                    GtkWidget       *entry);
static void      tag_renamer_edit_tags_activated   (ThunarxMenuItem *item,
                                                    ThunarxFileInfo *file);



struct _TagRenamerClass
{
  ThunarxRenamerClass __parent__;
};

struct _TagRenamer
{
  ThunarxRenamer __parent__;

  /* Properties */
  TagRenamerFormat  format;
  gchar            *text;
  gboolean          replace_spaces;
  gboolean          lowercase;
};



THUNARX_DEFINE_TYPE (TagRenamer, tag_renamer, THUNARX_TYPE_RENAMER);



static void
tag_renamer_class_init (TagRenamerClass *klass)
{
  ThunarxRenamerClass *thunarxrenamer_class;
  GObjectClass        *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = tag_renamer_finalize;
  gobject_class->get_property = tag_renamer_get_property;
  gobject_class->set_property = tag_renamer_set_property;

  thunarxrenamer_class = THUNARX_RENAMER_CLASS (klass);
  thunarxrenamer_class->process = tag_renamer_process;
  thunarxrenamer_class->get_menu_items = tag_renamer_get_menu_items;

  /**
   * TagRenamer:format:
   *
   * Naming format used for renaming the selected files.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FORMAT,
                                   g_param_spec_enum ("format",
                                                      "format",
                                                      "format",
                                                      TYPE_TAG_RENAMER_FORMAT,
                                                      TAG_RENAMER_FORMAT_TRACK_TITLE,
                                                      G_PARAM_READWRITE));

  /**
   * TagRenamer:text:
   *
   * Custom format from the user.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT,
                                   g_param_spec_string ("text",
                                                        "text",
                                                        "text",
                                                        NULL,
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

}



static void
tag_renamer_init (TagRenamer *tag_renamer)
{
  AtkRelationSet  *relations;
  AtkRelation     *relation;
  AtkObject       *object;
  GtkWidget       *grid;
  GtkWidget       *label;
  GtkWidget       *combo;
  GtkWidget       *image;
  GtkWidget       *button;
  GtkWidget       *entry;
  GEnumClass      *klass;
  GtkListStore    *store;
  GtkCellRenderer *cell;
  GtkTreeIter      iter;
  guint            n;

  grid = gtk_grid_new ();
  gtk_grid_set_row_spacing (GTK_GRID (grid), 6);
  gtk_grid_set_column_spacing (GTK_GRID (grid), 12);
  gtk_box_pack_start (GTK_BOX (tag_renamer), grid, FALSE, FALSE, 0);
  gtk_widget_show (grid);

  /* Custom format */
  label = gtk_label_new_with_mnemonic (_("Cust_om format:"));
  gtk_label_set_xalign (GTK_LABEL (label), 1.0f);
  gtk_grid_attach (GTK_GRID (grid), label, 0, 1, 1, 1);
  gtk_widget_show (label);

  entry = gtk_entry_new ();
  gtk_widget_set_hexpand (entry, TRUE);
  gtk_grid_attach (GTK_GRID (grid), entry, 1, 1, 1, 1);
  g_object_bind_property (G_OBJECT (entry), "text", G_OBJECT (tag_renamer), "text", G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
  gtk_widget_show (entry);

  button = gtk_button_new ();
  gtk_grid_attach (GTK_GRID (grid), button, 2, 1, 1, 1);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (tag_renamer_help_clicked), NULL);
  gtk_widget_show (button);

  image = gtk_image_new_from_icon_name ("dialog-information", GTK_ICON_SIZE_MENU);
  gtk_container_add (GTK_CONTAINER (button), image);
  gtk_widget_show (image);

  /* Set Atk label relation for the entry */
  object = gtk_widget_get_accessible (entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));
  g_object_unref (relations);

  /* Format label */
  label = gtk_label_new_with_mnemonic (_("_Format:"));
  gtk_label_set_xalign (GTK_LABEL (label), 1.0f);
  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
  gtk_widget_show (label);

  /* Create combo box store */
  store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

  /* Format combo box */
  combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (combo), tag_renamer_combo_separator_func, NULL, NULL);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK (tag_renamer_combo_changed), entry);

  /* Text renderer for the combo box */
  cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), cell, "text", COLUMN_TEXT, NULL);

  /* Insert items */
  klass = g_type_class_ref (TYPE_TAG_RENAMER_FORMAT);
  for (n = 0; n < klass->n_values; ++n)
    {
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, COLUMN_TEXT, _(klass->values[n].value_nick), COLUMN_NUMBER, n, -1);
    }
  g_type_class_unref (klass);
  g_object_bind_property (G_OBJECT (tag_renamer), "format", G_OBJECT (combo), "active", G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);
  gtk_widget_set_hexpand (combo, TRUE);
  gtk_grid_attach (GTK_GRID (grid), combo, 1, 0, 2, 1);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), combo);
  gtk_widget_show (combo);

  /* Release the store */
  g_object_unref (G_OBJECT (store));

  /* Set Atk label relation for the combo box */
  object = gtk_widget_get_accessible (combo);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));
  g_object_unref (relations);

  button = gtk_check_button_new_with_mnemonic (_("_Underscores"));
  g_object_bind_property (G_OBJECT (button), "active", G_OBJECT (tag_renamer), "replace-spaces", G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);
  gtk_widget_set_tooltip_text (GTK_WIDGET (button), _("Activating this option will replace all spaces in the target filename "
        "with underscores."));
  gtk_grid_attach (GTK_GRID (grid), button, 3, 0, 1, 1);
  gtk_widget_show (button);

  button = gtk_check_button_new_with_mnemonic (_("_Lowercase"));
  g_object_bind_property (G_OBJECT (button), "active", G_OBJECT (tag_renamer), "lowercase", G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);
  gtk_widget_set_tooltip_text (GTK_WIDGET (button), _("If you activate this, the resulting filename will only contain lowercase letters."));
  gtk_grid_attach (GTK_GRID (grid), button, 3, 1, 1, 1);
  gtk_widget_show (button);
}





static void
tag_renamer_finalize (GObject *object)
{
  TagRenamer *tag_renamer = TAG_RENAMER (object);

  /* Free string */
  g_free (tag_renamer->text);

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
    case PROP_FORMAT:
      g_value_set_enum (value, tag_renamer_get_format (tag_renamer));
      break;

    case PROP_TEXT:
      g_value_set_string (value, tag_renamer_get_text (tag_renamer));
      break;

    case PROP_REPLACE_SPACES:
      g_value_set_boolean (value, tag_renamer_get_replace_spaces (tag_renamer));
      break;

    case PROP_LOWERCASE:
      g_value_set_boolean (value, tag_renamer_get_lowercase (tag_renamer));
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
    case PROP_FORMAT:
      tag_renamer_set_format (tag_renamer, g_value_get_enum (value));
      break;

    case PROP_TEXT:
      tag_renamer_set_text (tag_renamer, g_value_get_string (value));
      break;

    case PROP_REPLACE_SPACES:
      tag_renamer_set_replace_spaces (tag_renamer, g_value_get_boolean (value));
      break;

    case PROP_LOWERCASE:
      tag_renamer_set_lowercase (tag_renamer, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gchar *
tag_renamer_process_format (TagRenamer      *tag_renamer,
                            ThunarxFileInfo *file,
                            const gchar     *format)
{
  TagLib_File *taglib_file;
  TagLib_Tag  *taglib_tag;
  const gchar *value;
  guint        integer;
  gchar       *uri, *filename;
  gchar       *string, *tmp;
  GString     *result;
  const gchar *p;

  /* Get the file uri, return on fail */
  uri = thunarx_file_info_get_uri (file);
  if (G_UNLIKELY (uri == NULL))
    return NULL;

  /* Conver the uri into a filename, return on fail */
  filename = g_filename_from_uri (uri, NULL, NULL);
  g_free (uri);
  if (G_UNLIKELY (filename == NULL))
    return NULL;

  /* Load taglib file, return on fail */
  taglib_file = taglib_file_new (filename);
  g_free (filename);
  if (G_UNLIKELY (taglib_file == NULL))
    return NULL;

  /* Load tag info, return on fail */
  taglib_tag = taglib_file_tag (taglib_file);
  if (G_UNLIKELY (taglib_tag == NULL))
    {
      taglib_file_free (taglib_file);
      return NULL;
    }

  /* Create string with some size */
  result = g_string_sized_new (512);

  /* walk the string */
  for (p = format; *p != '\0'; ++p)
    {
      if (p[0] == '%' && p[1] != '\0')
        {
          switch (*++p)
            {
            case 'a':
              /* Arist */
              value = taglib_tag_artist (taglib_tag);
              if (G_UNLIKELY (g_utf8_strlen (value, -1) == 0))
                value = _("Unknown Artist");
              result = g_string_append (result, value);
              break;

            case 't':
              /* Title */
              value = taglib_tag_title (taglib_tag);
              if (G_UNLIKELY (g_utf8_strlen (value, -1) == 0))
                value = _("Unknown Title");
              result = g_string_append (result, value);
              break;

            case 'b':
              /* Album */
              value = taglib_tag_album (taglib_tag);
              if (G_LIKELY (g_utf8_strlen (value, -1) > 0))
                result = g_string_append (result, value);
              break;

            case 'g':
              /* Genre */
              value = taglib_tag_genre (taglib_tag);
              if (G_LIKELY (g_utf8_strlen (value, -1) > 0))
                result = g_string_append (result, value);
              break;

            case 'n':
              /* Track Number */
              integer = taglib_tag_track (taglib_tag);
              g_string_append_printf (result, "%02d", integer);
              break;

            case 'y':
              /* Year */
              integer = taglib_tag_year (taglib_tag);
              if (G_LIKELY (integer > 0))
                g_string_append_printf (result, "%d", integer);
              break;

            case 'c':
              /* Comment */
              value = taglib_tag_comment (taglib_tag);
              if (G_LIKELY (g_utf8_strlen (value, -1) > 0))
                result = g_string_append (result, value);
              break;

            case '%':
              /* Percentage character */
              result = g_string_append_c (result, '%');
              break;
            }
        }
      else
        {
          /* Append character */
          result = g_string_append_c (result, *p);
        }
    }

  /* Free tag info strings */
  taglib_tag_free_strings ();

  /* Cleanup */
  taglib_file_free (taglib_file);

  /* Get the resulting string */
  string = g_string_free (result, FALSE);

  /* Replace spaces if requested */
  if (tag_renamer_get_replace_spaces (tag_renamer))
    string = g_strdelimit (string, " ", '_');

  /* Convert to lowercase if requested */
  if (tag_renamer_get_lowercase (tag_renamer))
  {
    tmp = g_utf8_strdown (string, -1);
    g_free (string);
    string = tmp;
  }

  /* Replace slashes */
  string = g_strdelimit (string, G_DIR_SEPARATOR_S, '_');

  /* Return the result */
  return string;
}



static gchar*
tag_renamer_process (ThunarxRenamer  *renamer,
                     ThunarxFileInfo *file,
                     const gchar     *text,
                     guint            idx)
{
  TagRenamer  *tag_renamer = TAG_RENAMER (renamer);
  const gchar *format;

  switch (tag_renamer_get_format (tag_renamer))
    {
    case TAG_RENAMER_FORMAT_TRACK_DOT_TITLE:
      format = "%n. %t";
      break;

    case TAG_RENAMER_FORMAT_TRACK_DOT_ARTIST_TITLE:
      format = "%n. %a - %t";
      break;

    case TAG_RENAMER_FORMAT_TRACK_TITLE:
      format = "%n - %t";
      break;

    case TAG_RENAMER_FORMAT_TRACK_ARTIST_TITLE:
      format = "%n - %a - %t";
      break;

    case TAG_RENAMER_FORMAT_TITLE:
      format = "%t";
      break;

    case TAG_RENAMER_FORMAT_ARTIST_TITLE:
      format = "%a - %t";
      break;

    case TAG_RENAMER_FORMAT_ARTIST_TRACK_TITLE:
      format = "%a - %n - %t";
      break;

    default: /* Custom & separator */
      format = tag_renamer_get_text (tag_renamer);
      if (format == NULL || *format == '\0')
        return g_strdup (text);
      break;
    }

  return tag_renamer_process_format (tag_renamer, file, format);
}



static GList*
tag_renamer_get_menu_items (ThunarxRenamer *renamer,
                            GtkWindow      *window,
                            GList          *files)
{
  ThunarxMenuItem *item;
  GList           *file;
  GList           *items = NULL;
  ThunarxFileInfo *info;

  if (g_list_length (files) != 1)
    return NULL;

  file = g_list_first (files);

  if (G_UNLIKELY (file == NULL))
    return NULL;

  info = THUNARX_FILE_INFO (file->data);

  if (G_LIKELY (media_tags_get_audio_file_supported (info)))
    {
      /* Edit tags action */
      item = thunarx_menu_item_new ("edit-tags", _("Edit _Tags"), _("Edit ID3/OGG tags of this file."), "gtk-edit");
      g_object_set_data_full (G_OBJECT (item), "window", g_object_ref (G_OBJECT (window)), (GDestroyNotify)g_object_unref);
      g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (tag_renamer_edit_tags_activated), info);
      items = g_list_prepend (items, item);
    }

  return items;
}



static void
tag_renamer_help_clicked (GtkWidget *button)
{
  GtkWidget *dialog;

  /* Create dialog */
  dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (button)),
                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, _("Tag Help"));

  /* Format help text */
  gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                              "<tt>%%a = </tt>%s\n"
                                              "<tt>%%t = </tt>%s\n"
                                              "<tt>%%b = </tt>%s\n"
                                              "<tt>%%g = </tt>%s\n"
                                              "<tt>%%n = </tt>%s\n"
                                              "<tt>%%y = </tt>%s\n"
                                              "<tt>%%c = </tt>%s",
                                              _("Artist"), _("Title"),
                                              _("Album"), _("Genre"),
                                              _("Track number"), _("Year"),
                                              _("Comment"));

  /* Run and destroy */
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}



static gboolean
tag_renamer_combo_separator_func (GtkTreeModel *model,
                                  GtkTreeIter  *iter,
                                  gpointer      user_data)
{
  gint n;

  /* Get item number from model */
  gtk_tree_model_get (model, iter, COLUMN_NUMBER, &n, -1);

  /* Return true if this is the separator item */
  return (n == TAG_RENAMER_FORMAT_SEPARATOR);
}



static void
tag_renamer_combo_changed (GtkComboBox *combo_box,
                           GtkWidget   *entry)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  gint          n;

  /* Get the combo model and active iter*/
  model = gtk_combo_box_get_model (combo_box);
  gtk_combo_box_get_active_iter (combo_box, &iter);

  /* Get item number from model */
  gtk_tree_model_get (model, &iter, COLUMN_NUMBER, &n, -1);

  /* Whether the entry is sensitive */
  gtk_widget_set_sensitive (entry, (n == TAG_RENAMER_FORMAT_CUSTOM));
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
                       "name", _("Audio Tags"),
                       NULL);
}



/**
 * tag_renamer_get_format:
 * @tag_renamer : a #TagRenamer.
 *
 * Returns the current #TagRenamerFormat
 * for the @tag_renamer.
 *
 * Return value: the current naming format of @tag_renamer.
 **/
TagRenamerFormat
tag_renamer_get_format (TagRenamer *tag_renamer)
{
  g_return_val_if_fail (IS_TAG_RENAMER (tag_renamer), TAG_RENAMER_FORMAT_TRACK_TITLE);
  return tag_renamer->format;
}



/**
 * tag_renamer_set_format:
 * @tag_renamer : a #TagRenamer.
 * @format      : the new #TagRenamerFormat.
 *
 * Sets the #TagRenamerFormat of the @tag_renamer
 * to the specified @format.
 **/
void
tag_renamer_set_format (TagRenamer      *tag_renamer,
                        TagRenamerFormat format)
{
  g_return_if_fail (IS_TAG_RENAMER (tag_renamer));

  /* Check if we already use that format */
  if (G_UNLIKELY (tag_renamer->format == format))
    return;

  /* Apply the new format */
  tag_renamer->format = format;

  /* Notify listeners */
  g_object_notify (G_OBJECT (tag_renamer), "format");

  /* Emit the "changed" signal */
  thunarx_renamer_changed (THUNARX_RENAMER (tag_renamer));
}



const gchar *
tag_renamer_get_text (TagRenamer *tag_renamer)
{
  g_return_val_if_fail (IS_TAG_RENAMER (tag_renamer), NULL);
  return tag_renamer->text;
}



void
tag_renamer_set_text (TagRenamer  *tag_renamer,
                      const gchar *text)
{
  g_return_if_fail (IS_TAG_RENAMER (tag_renamer));

  /* check if we have a new text */
  if (G_LIKELY (g_strcmp0 (tag_renamer->text, text) != 0))
    {
      /* apply the new text */
      g_free (tag_renamer->text);
      tag_renamer->text = g_strdup (text);

      /* update the renamer */
      thunarx_renamer_changed (THUNARX_RENAMER (tag_renamer));

      /* notify listeners */
      g_object_notify (G_OBJECT (tag_renamer), "text");
    }
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



static void
tag_renamer_edit_tags_activated (ThunarxMenuItem *item,
                                 ThunarxFileInfo *file)
{
  GtkWindow *window;
  GtkWidget *dialog;

  /* Determine the parent window */
  window = g_object_get_data (G_OBJECT (item), "window");
  if (G_UNLIKELY (window == NULL))
    return;

  dialog = audio_tags_page_dialog_new (window, file);

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}
