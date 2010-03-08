# $Id$

INCLUDES =								\
	-I$(top_builddir)						\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"thunar-media-tags-plugin\"			\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	$(PLATFORM_CPPFLAGS)

extensionsdir = $(libdir)/thunarx-2
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
	$(EXO_CFLAGS)							\
	$(PLATFORM_CFLAGS)						\
	$(THUNARX_CFLAGS)						\
	$(TAGLIB_CFLAGS)						\
	$(TAGLIBC_CFLAGS)

thunar_media_tags_plugin_la_LDFLAGS =					\
	-avoid-version							\
	-export-dynamic							\
	-module								\
	$(EXO_LIBS)							\
	$(THUNARX_LIBS)							\
	$(TAGLIB_LIBS)							\
	$(TAGLIBC_LIBS)							\
	$(PLATFORM_LDFLAGS)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
