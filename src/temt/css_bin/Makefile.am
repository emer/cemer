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

css@PDP_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/temt/lib/libtemt@PDP_SUFFIX@-@PACKAGE_VERSION@.la

css@PDP_SUFFIX@_CPPFLAGS = @PDP_INCLUDES@
css@PDP_SUFFIX@_CXXFLAGS = @OPT_LEVEL@
