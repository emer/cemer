#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = leabra
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/leabra.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

pdp_TA_type.h:
	cd ../pdp && ${MAKE} pdp_TA_type.h

noinst_lib_LTLIBRARIES = libleabra@PDP_SUFFIX@.la

libleabra@PDP_SUFFIX@_la_SOURCES =\
	leabra_TA.cpp \
	leabra_TA_type.h \
	leabra_TA_inst.h

libleabra@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@srcdir@/../ \
	-I@abs_top_builddir@ \
	-I@srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../taiqtso \
	-I@srcdir@/../pdp_lib

libleabra@PDP_SUFFIX@_la_CXXFLAGS = -O0

BUILT_SOURCES = pdp_TA_type.h leabra_TA_type.h
