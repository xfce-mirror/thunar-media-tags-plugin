# $Id$

AM_CPPFLAGS =								\
	-I$(top_builddir)						\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"thunar-media-tags-plugin\"			\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	$(PLATFORM_CPPFLAGS)

extensionsdir = $(libdir)/thunarx-3
extensions_LTLIBRARIES =						\
	thunar-media-tags-plugin.la

thunar_media_tags_plugin_la_SOURCES =					\
	media-tags-provider.h						\
	media-tags-provider.c						\
	tag-renamer.h							\
	tag-renamer.c							\
	audio-tags-page.h						\
	audio-tags-page.c						\
	media-tags-plugin.c

thunar_media_tags_plugin_la_CFLAGS =					\
	$(PLATFORM_CFLAGS)						\
	$(LIBXFCE4UTIL_CFLAGS)						\
	$(THUNARX_CFLAGS)						\
	$(TAGLIB_CFLAGS)						\
	$(TAGLIBC_CFLAGS)

thunar_media_tags_plugin_la_LDFLAGS =					\
	-avoid-version							\
	-export-dynamic							\
	-module								\
	$(PLATFORM_LDFLAGS)

thunar_media_tags_plugin_la_LIBADD =					\
	$(LIBXFCE4UTIL_LIBS)						\
	$(THUNARX_LIBS)							\
	$(TAGLIB_LIBS)							\
	$(TAGLIBC_LIBS)

EXTRA_DIST =								\
	meson.build

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
