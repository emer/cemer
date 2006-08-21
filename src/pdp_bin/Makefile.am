#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
include $(top_srcdir)/mfstamp.am

PROJNAME = pdp_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# pdp++
bin_PROGRAMS = pdp@PDP_SUFFIX@++

pdp@PDP_SUFFIX@___SOURCES = main.cpp

# toggle creation of ta library. see configure.ac/acsite.m4
if LIBTA
pdp@PDP_SUFFIX@___LDADD =\
	@abs_top_builddir@/src/pdp_lib/libpdp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta_lib/libta@PDP_SUFFIX@.la
else
pdp@PDP_SUFFIX@___LDADD =\
	@abs_top_builddir@/src/pdp_lib/libpdp@PDP_SUFFIX@.la
endif


pdp@PDP_SUFFIX@___CPPFLAGS =\
	-I@abs_top_builddir@ \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../pdp \
	-I@srcdir@/../taiqtso \
	-I@srcdir@/../ta


pdp@PDP_SUFFIX@___CXXFLAGS = @OPT_LEVEL@

