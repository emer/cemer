#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = cs
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	../../temt/ta/ta_TA_type.h \
	../network/emergent_TA_type.h \
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
	libcs_EmergentToolkit@EMERGENT_SUFFIX@.la \
	libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la

libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libcs_EmergentToolkit@EMERGENT_SUFFIX@.la

libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libcs_EmergentToolkit@EMERGENT_SUFFIX@.la

libcs_EmergentToolkit@EMERGENT_SUFFIX@_la_SOURCES =\
	cs_TA.cpp \
	cs_TA_type.h \
	cs_TA_inst.h

libcs_EmergentToolkit@EMERGENT_SUFFIX@_la_CPPFLAGS = $(libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libcs_EmergentToolkit@EMERGENT_SUFFIX@_la_CXXFLAGS = -O0

cs_def.h:
