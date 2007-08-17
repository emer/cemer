#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
include $(top_srcdir)/mfstamp.am

PROJNAME = css_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# emergent
bin_PROGRAMS = css@EMERGENT_SUFFIX@

css@EMERGENT_SUFFIX@_SOURCES = main.cpp

css@EMERGENT_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/temt/lib/libtemt@EMERGENT_SUFFIX@-@PACKAGE_VERSION@.la

css@EMERGENT_SUFFIX@_CPPFLAGS = @EMERGENT_INCLUDES@
css@EMERGENT_SUFFIX@_CXXFLAGS = @OPT_LEVEL@
