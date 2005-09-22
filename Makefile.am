# SUBDIRS instruction the autotools
# which directories have Makefile.am's in them.
# If you remove/add one of SUBDIRS, be sure to
# do the same at the bottom of configure.ac

SUBDIRS =\
	src/maketa \
	src/taiqtso \
	src/ta \
	src/css \
	src/tamisc \
	src/pdp \
	src/leabra \
	src/bp

EXTRA_DIST = src/ta/ios-g++-3.1

# These actions occur immediately before a 
# distribution tarball is created.

dist-hook:
	rm -fr $(distdir)/src/ta/ios-g++-3.1/.svn
#	cp -p $(srcdir)/src/ta/ios-g++-3.1/fstream.h \
#		$(srcdir)/src/ta/ios-g++-3.1/iostream.h \
#		$(srcdir)/src/ta/ios-g++-3.1/sstream.h \
#		$(srcdir)/src/ta/ios-g++-3.1/streambuf.h \
#		$(srcdir)/src/ta/ios-g++-3.1/strstream.h \
#		$(distdir)/src/ta/ios-g++-3.1
	cp -p $(srcdir)/src/ta/taglobal.h \
		$(srcdir)/src/ta/platform.h \
		$(srcdir)/src/ta/qtdefs.h\
		$(distdir)/src/ta
	cp -p $(srcdir)/src/tamisc/tamisc_maketa_defs.h \
		$(distdir)/src/tamisc/tamisc_maketa_defs.h
	cp -p $(srcdir)/src/ta/ta_maketa_defs.h \
		$(distdir)/src/ta/ta_maketa_defs.h