#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = bp
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	../ta/ta_TA_type.h \
	../emergent/emergent_TA_type.h \
	bp.h \
	rbp.h \
	bp_v3_compat.h

HEADS=\
	bp.h \
	rbp.h \
	bp_v3_compat.h


SRCS=\
	bp.cpp \
	rbp.cpp  \
	bp_v3_compat.cpp

include_HEADERS = $(HEADS) $(TAHEADS) bp_TA_type.h bp_TA_inst.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libbp_EmergentToolkit@PDP_SUFFIX@.la \
	libbp@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libbp@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libbp@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @PDP_INCLUDES@
libbp@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libbp@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libbp_EmergentToolkit@PDP_SUFFIX@.la

libbp@PDP_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libbp_EmergentToolkit@PDP_SUFFIX@.la

libbp_EmergentToolkit@PDP_SUFFIX@_la_SOURCES =\
	bp_TA.cpp \
	bp_TA_type.h \
	bp_TA_inst.h

libbp_EmergentToolkit@PDP_SUFFIX@_la_CPPFLAGS = $(libbp@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libbp_EmergentToolkit@PDP_SUFFIX@_la_CXXFLAGS = -O0

bp_def.h:
