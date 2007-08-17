#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
include $(top_srcdir)/mfstamp.am
PROJNAME = emergent_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# emergent
bin_PROGRAMS = emergent@EMERGENT_SUFFIX@
emergent@EMERGENT_SUFFIX@_SOURCES = main.cpp

emergent@EMERGENT_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/emergent/lib/libemergent@EMERGENT_SUFFIX@-@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/temt/lib/libtemt@EMERGENT_SUFFIX@-@PACKAGE_VERSION@.la

emergent@EMERGENT_SUFFIX@_CPPFLAGS = @EMERGENT_INCLUDES@
emergent@EMERGENT_SUFFIX@_CXXFLAGS = @OPT_LEVEL@

