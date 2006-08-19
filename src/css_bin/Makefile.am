#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
include $(top_srcdir)/mfstamp.am

PROJNAME = css_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# pdp++
bin_PROGRAMS = css@PDP_SUFFIX@++

css@PDP_SUFFIX@___SOURCES = main.cpp

if LIBTA
css@PDP_SUFFIX@___LDADD =\
	@abs_top_builddir@/src/ta_lib/libta4@PDP_SUFFIX@.la
else
css@PDP_SUFFIX@___LDADD =\
	@abs_top_builddir@/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@.la

if TA_GUI
css@PDP_SUFFIX@___LDADD +=\
	@abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la
endif

endif
css@PDP_SUFFIX@___CPPFLAGS = \
	-I@abs_top_builddir@ \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso \
	-I@srcdir@/../ta

css@PDP_SUFFIX@___CXXFLAGS = @OPT_LEVEL@
