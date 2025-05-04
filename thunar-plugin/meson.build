thunar_media_tags_sources = [
  'audio-tags-page.c',
  'audio-tags-page.h',
  'media-tags-plugin.c',
  'media-tags-provider.c',
  'media-tags-provider.h',
  'tag-renamer.c',
  'tag-renamer.h',
]

shared_module(
  'thunar-media-tags-plugin',
  thunar_media_tags_sources,
  gnu_symbol_visibility: 'hidden',
  dependencies: [
    glib,
    gtk,
    libxfce4util,
    taglib_c,
    taglib,
    thunarx,
  ],
  name_prefix: '',
  install: true,
  install_dir: get_option('prefix') / get_option('libdir') / 'thunarx-3',
)
