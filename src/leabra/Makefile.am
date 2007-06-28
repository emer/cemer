#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = leabra
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	../ta/ta_TA_type.h \
	../pdp/pdp_TA_type.h \
	leabra.h \
	leabra_extra.h \
	leabra_td.h \
	leabra_pvlv.h \
	leabra_pbwm.h \
	leabra_v3_compat.h

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

include_HEADERS = $(HEADS) $(TAHEADS) leabra_TA_type.h leabra_TA_inst.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libleabra_TypeAccess@PDP_SUFFIX@.la \
	libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)
libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @PDP_INCLUDES@
libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libleabra_TypeAccess@PDP_SUFFIX@.la
libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libleabra_TypeAccess@PDP_SUFFIX@.la
libleabra_TypeAccess@PDP_SUFFIX@_la_SOURCES =\
	leabra_TA.cpp \
	leabra_TA_type.h \
	leabra_TA_inst.h
libleabra_TypeAccess@PDP_SUFFIX@_la_CPPFLAGS = $(libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)
libleabra_TypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -O0

leabra_def.h:
