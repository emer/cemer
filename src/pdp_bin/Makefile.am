#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = pdp_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# pdp++
bin_PROGRAMS = pdp@PDP_SUFFIX@++

pdp@PDP_SUFFIX@___SOURCES = main.cpp

pdp@PDP_SUFFIX@___LDADD =\
	@abs_top_builddir@/src/pdp_lib/libpdp4@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta_lib/libta4@PDP_SUFFIX@.la

pdp@PDP_SUFFIX@___CPPFLAGS =\
	-I@abs_top_builddir@ \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../pdp \
	-I@srcdir@/../taiqtso \
	-I@srcdir@/../ta


pdp@PDP_SUFFIX@___CXXFLAGS = @OPT_LEVEL@

