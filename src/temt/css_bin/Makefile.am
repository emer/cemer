#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
include $(top_srcdir)/mfstamp.am

PROJNAME = css_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# emergent
bin_PROGRAMS = css@PDP_SUFFIX@

css@PDP_SUFFIX@_SOURCES = main.cpp

if LIBTA
css@PDP_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/temt/ta_lib/libtacss@PDP_SUFFIX@-@PACKAGE_VERSION@.la
else
css@PDP_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/temt/tamisc/libtamisc@PDP_SUFFIX@-@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/temt/css/libcss@PDP_SUFFIX@-@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/temt/ta/libta@PDP_SUFFIX@-@PACKAGE_VERSION@.la

if TA_GUI
css@PDP_SUFFIX@_LDADD +=\
	@abs_top_builddir@/src/temt/taiqtso/libtaiqtso@PDP_SUFFIX@_@PACKAGE_VERSION@.la
endif
endif

css@PDP_SUFFIX@_CPPFLAGS = @PDP_INCLUDES@
css@PDP_SUFFIX@_CXXFLAGS = @OPT_LEVEL@
