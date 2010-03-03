CLEANFILES= *~
MAINTAINERCLEANFILES= configure config.h config.h.in Makefile Makefile.in

ACLOCAL_AMFLAGS= -I m4
SUBDIRS= src

# other files to distribute: (pending)
EXTRA_DIST = AUTHORS ChangeLog COPYING INSTALL LICENSE NEWS \
             README THANKS autogen.sh configure.ac configure \
             docs books locale config res scores templates xrc packages

install-data-local:
	cp -R $(top_srcdir)/res/ $(DESTDIR)@datarootdir@/lenmus
	find $(DESTDIR)@datarootdir@/lenmus -type d -name .svn | xargs rm -rf

uninstall-local:
	rm -rf $(DESTDIR)@datarootdir@/lenmus
