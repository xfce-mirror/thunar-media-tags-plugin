# $Id$

INCLUDES =								\
	-I$(top_builddir)						\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"thunar-tag-renamer-plugin\"			\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	$(PLATFORM_CPPFLAGS)

extensionsdir = $(libdir)/thunarx-1
extensions_LTLIBRARIES =						\
	thunar-tag-renamer-plugin.la

thunar_tag_renamer_plugin_la_SOURCES =					\
	tag-renamer-provider.h						\
	tag-renamer-provider.c						\
	tag-renamer.h							\
	tag-renamer.c							\
	tag-renamer-property-page.h					\
	tag-renamer-property-page.c					\
	tag-renamer-plugin.c

thunar_tag_renamer_plugin_la_CFLAGS =					\
	$(EXO_CFLAGS)							\
	$(PLATFORM_CFLAGS)						\
	$(THUNARX_CFLAGS)						\
	$(TAGLIB_CFLAGS)						\
	$(TAGLIBC_CFLAGS)

thunar_tag_renamer_plugin_la_LDFLAGS =					\
	-avoid-version							\
	-export-dynamic							\
	-module								\
	$(THUNARX_LIBS)							\
	$(TAGLIB_LIBS)							\
	$(TAGLIBC_LIBS)							\
	$(PLATFORM_LDFLAGS)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
