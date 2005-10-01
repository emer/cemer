# SUBDIRS tells the autotools
# which directories have Makefile.am's in them.
# If you remove/add one of SUBDIRS, be sure to
# do the same at the bottom of configure.ac

dist_pkgdata_DATA = AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README

DIST_SUBDIRS =src/maketa src/taiqtso src/ta src/css src/tamisc src/pdp src/leabra src/bp
SUBDIRS = src/maketa @WANT_TAIQTSO@ src/ta src/css src/tamisc src/pdp src/leabra src/bp

EXTRA_DIST = src/ta/ios-g++-3.1

# These actions occur immediately before a distribution tarball is created.

dist-hook:
	rm -fr $(distdir)/src/ta/ios-g++-3.1/.svn
	cp -p $(srcdir)/src/ta/taglobal.h \
		$(srcdir)/src/ta/platform.h \
		$(srcdir)/src/ta/qtdefs.h\
		$(distdir)/src/ta
	cp -p $(srcdir)/src/tamisc/tamisc_maketa_defs.h \
		$(distdir)/src/tamisc/tamisc_maketa_defs.h
	cp -p $(srcdir)/src/ta/ta_maketa_defs.h \
		$(distdir)/src/ta/ta_maketa_defs.h


# Counts the number of lines in *.h and *.cpp files

linecount:
	@ wc --lines `find -name "*.cpp" -printf '%p ' -o -name "*.h" -printf '%p '` | grep total
