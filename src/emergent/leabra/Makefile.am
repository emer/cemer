#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = leabra
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	../../temt/ta/ta_TA_type.h \
	../network/emergent_TA_type.h \
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

include_HEADERS = $(HEADS) $(TAHEADS) leabra_TA_type.h leabra_TA_inst.h leabra_def.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libleabra_EmergentToolkit@EMERGENT_SUFFIX@.la \
	libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la

libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)
libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libleabra_EmergentToolkit@EMERGENT_SUFFIX@.la
libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libleabra_EmergentToolkit@EMERGENT_SUFFIX@.la
libleabra_EmergentToolkit@EMERGENT_SUFFIX@_la_SOURCES =\
	leabra_TA.cpp \
	leabra_TA_type.h \
	leabra_TA_inst.h
libleabra_EmergentToolkit@EMERGENT_SUFFIX@_la_CPPFLAGS = $(libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)
libleabra_EmergentToolkit@EMERGENT_SUFFIX@_la_CXXFLAGS = -O0

clean-local:
	-rm -f libleabra*
	if [ -n "`ls *\.moc\.cpp`" ];then rm `ls *\.moc\.cpp`;fi

leabra_def.h:
