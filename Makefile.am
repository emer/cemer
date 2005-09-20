#SUBDIRS = src/maketa src/taiqtso src/tacss src/tamisc src/pdp src/leabra src/bp
SUBDIRS = src/maketa src/taiqtso src/ta src/css src/tamisc

EXTRA_DIST =\
	src/ta/ta_maketa_defs.h

dist-hook:
	mkdir $(distdir)/src/ta/ios-g++-3.1
	cp -p $(srcdir)/src/ta/ios-g++-3.1/fstream.h \
		$(srcdir)/src/ta/ios-g++-3.1/iostream.h \
		$(srcdir)/src/ta/ios-g++-3.1/sstream.h \
		$(srcdir)/src/ta/ios-g++-3.1/streambuf.h \
		$(srcdir)/src/ta/ios-g++-3.1/strstream.h \
		$(distdir)/src/ta/ios-g++-3.1
	cp -p $(srcdir)/src/ta/taglobal.h \
		$(srcdir)/src/ta/platform.h \
		$(srcdir)/src/ta/qtdefs.h\
		$(distdir)/src/ta
	cp -p $(srcdir)/src/tamisc/tamisc_maketa_defs.h \
		$(distdir)/src/tamisc/tamisc_maketa_defs.h