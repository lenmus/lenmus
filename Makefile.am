CLEANFILES= *~
MAINTAINERCLEANFILES= configure config.h config.h.in Makefile Makefile.in

ACLOCAL_AMFLAGS= -I m4
SUBDIRS= src

# other files to distribute: (pending)
EXTRA_DIST = AUTHORS ChangeLog COPYING INSTALL LICENSE NEWS \
             README THANKS autogen.sh configure.ac configure \
             docs books locale config res scores templates xrc packages

install-data-local:
	$(INSTALL) -d $(DESTDIR)@datarootdir@/lenmus
	cp -R $(top_srcdir)/xrc/ $(DESTDIR)@datarootdir@/lenmus
	cp -R $(top_srcdir)/res/ $(DESTDIR)@datarootdir@/lenmus
	cp -R $(top_srcdir)/locale/ $(DESTDIR)@datarootdir@/lenmus
	cp -R $(top_srcdir)/books/ $(DESTDIR)@datarootdir@/lenmus
	cp -R $(top_srcdir)/templates/ $(DESTDIR)@datarootdir@/lenmus
	cp -R $(top_srcdir)/test-scores/ $(DESTDIR)@datarootdir@/lenmus

uninstall-local:
	rm -rf $(DESTDIR)@datarootdir@/lenmus
