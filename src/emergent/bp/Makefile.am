#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = bp
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	../../temt/ta/ta_TA_type.h \
	../network/emergent_TA_type.h \
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
	libbp_EmergentToolkit@EMERGENT_SUFFIX@.la \
	libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la

libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libbp_EmergentToolkit@EMERGENT_SUFFIX@.la

libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libbp_EmergentToolkit@EMERGENT_SUFFIX@.la

libbp_EmergentToolkit@EMERGENT_SUFFIX@_la_SOURCES =\
	bp_TA.cpp \
	bp_TA_type.h \
	bp_TA_inst.h

libbp_EmergentToolkit@EMERGENT_SUFFIX@_la_CPPFLAGS = $(libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libbp_EmergentToolkit@EMERGENT_SUFFIX@_la_CXXFLAGS = -O0

bp_def.h:
