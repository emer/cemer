#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = cs
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	../ta/ta_TA_type.h \
	../pdp/pdp_TA_type.h \
	cs.h \
	cs_v3_compat.h

HEADS=\
	cs.h \
	cs_v3_compat.h


SRCS=\
	cs.cpp \
	cs_v3_compat.cpp

include_HEADERS = $(HEADS) $(TAHEADS) cs_TA_type.h cs_TA_inst.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libcs_TypeAccess@PDP_SUFFIX@.la \
	libcs@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libcs@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libcs@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @PDP_INCLUDES@
libcs@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libcs@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libcs_TypeAccess@PDP_SUFFIX@.la

libcs@PDP_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libcs_TypeAccess@PDP_SUFFIX@.la

libcs_TypeAccess@PDP_SUFFIX@_la_SOURCES =\
	cs_TA.cpp \
	cs_TA_type.h \
	cs_TA_inst.h

libcs_TypeAccess@PDP_SUFFIX@_la_CPPFLAGS = $(libcs@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libcs_TypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -O0

cs_def.h:
