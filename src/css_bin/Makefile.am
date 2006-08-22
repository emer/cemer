#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
include $(top_srcdir)/mfstamp.am

PROJNAME = css_bin
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

# pdp++
bin_PROGRAMS = css@PDP_SUFFIX@

css@PDP_SUFFIX@_SOURCES = main.cpp

if LIBTA
css@PDP_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/ta_lib/libtacss@PDP_SUFFIX@@PDP_VERSION@.la
else
css@PDP_SUFFIX@_LDADD =\
	@abs_top_builddir@/src/tamisc/libtamisc@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@@PDP_VERSION@.la

if TA_GUI
css@PDP_SUFFIX@_LDADD +=\
	@abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@@PDP_VERSION@.la
endif

endif
css@PDP_SUFFIX@_CPPFLAGS = \
	-I@abs_top_builddir@ \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso \
	-I@srcdir@/../ta

css@PDP_SUFFIX@_CXXFLAGS = @OPT_LEVEL@
