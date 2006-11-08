#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = leabra
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/../leabra/leabra.h \
	@abs_srcdir@/../leabra/leabra_extra.h \
	@abs_srcdir@/../leabra/leabra_td.h \
	@abs_srcdir@/../leabra/leabra_pvlv.h \
	@abs_srcdir@/../leabra/leabra_pbwm.h \
	@abs_srcdir@/../leabra/leabra_v3_compat.h

HEADS=\
	leabra.h \
	leabra_extra.h \
	leabra_td.h \
	leabra_pvlv.h \
	leabra_pbwm.h \
	leabra_v3_compat.h

SRCS=\
	leabra.cpp \
	leabra_extra.cpp \
	leabra_td.cpp \
	leabra_pvlv.cpp \
	leabra_pbwm.cpp \
	leabra_v3_compat.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libleabra_TypeAccess@PDP_SUFFIX@.la \
	libleabra@PDP_SUFFIX@@PDP_VERSION@.la

libleabra@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libleabra@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS =\
	-I@srcdir@/.. \
	-I@abs_top_builddir@ \
	-I@abs_srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../pdp \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso

libleabra@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libleabra@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD = libleabra_TypeAccess@PDP_SUFFIX@.la

libleabra@PDP_SUFFIX@@PDP_VERSION@_la_DEPENDENCIES = libleabra_TypeAccess@PDP_SUFFIX@.la

libleabra_TypeAccess@PDP_SUFFIX@_la_SOURCES =\
	leabra_TA.cpp \
	leabra_TA_type.h \
	leabra_TA_inst.h

libleabra_TypeAccess@PDP_SUFFIX@_la_CPPFLAGS = $(libleabra@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS)

libleabra_TypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -O0
