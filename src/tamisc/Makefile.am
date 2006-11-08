#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = tamisc
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI


endif


include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libtamisc@PDP_SUFFIX@@PDP_VERSION@.la \
	libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_DEPENDENCIES = libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = $(SRCS) $(HEADS)

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS =\
	-I@srcdir@/../ \
	-I@abs_top_builddir@ \
	-I@srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD =\
	libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = tamisc_TA.cpp tamisc_TA_type.h tamisc_TA_inst.h

libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS = $(libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS)

libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = -g -O0
