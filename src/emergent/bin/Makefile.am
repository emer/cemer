#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
include $(top_srcdir)/mfstamp.am
PROJNAME = emergent_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# emergent++
bin_PROGRAMS = emergent@PDP_SUFFIX@
emergent@PDP_SUFFIX@_SOURCES = main.cpp

# toggle creation of ta library. see configure.ac/acsite.m4
if LIBTA
emergent@PDP_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/emergent/lib/libemergent@PDP_SUFFIX@-@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/temt/lib/libtacss@PDP_SUFFIX@-@PACKAGE_VERSION@.la
else
emergent@PDP_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/emergent/lib/libemergent@PDP_SUFFIX@-@PACKAGE_VERSION@.la
endif

emergent@PDP_SUFFIX@_CPPFLAGS = @PDP_INCLUDES@
emergent@PDP_SUFFIX@_CXXFLAGS = @OPT_LEVEL@

