# $Id$

SUBDIRS =								\
	po								\
	thunar-tag-renamer-plugin

AUTOMAKE_OPTIONS =							\
	1.8								\
	dist-bzip2

distclean-local:
	rm -rf *.spec *.cache *~

rpm: dist
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
	@rm -f $(PACKAGE)-$(VERSION).tar.gz

EXTRA_DIST =								\
	README								\
	THANKS								\
	AUTHORS								\
	COPYING								\
	NEWS								\
	intltool-extract.in						\
	intltool-merge.in						\
	intltool-update.in

DISTCLEANFILES = 							\
	intltool-extract						\
	intltool-merge							\
	intltool-update							\
	po/POTFILES

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
