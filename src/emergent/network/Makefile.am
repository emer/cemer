#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = emergent
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI =\
	pdpview_qt.h \
	pdpserver_qt.h \
	netstru_so.h \
	netstru_qtso.h \
	pdp_qtso.h \
	pdp_program_qtso.h 

HEADS_GUI =\
	pdpview_qt.h \
	pdpserver_qt.h \
	netstru_so.h \
	netstru_qtso.h \
	pdp_qtso.h \
	pdp_program_qtso.h 

SRCS_GUI =\
	pdpview_qt.cpp \
	pdpserver_qt.cpp \
	netstru_so.cpp \
	pdp_qtso.cpp \
	netstru_qtso.cpp \
	netstru_qtso.moc.cpp \
	pdp_program_qtso.cpp

endif

TAHEADS =\
	../../temt/ta/ta_TA_type.h \
	pdpdeclare.h \
	pdp_base.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	netdata.h \
	pdp_program.h \
	pdp_project.h \
	pdpserver.h \
	$(TAHEADS_GUI) \
	v3_compat.h

HEADS =\
	pdpdeclare.h \
	pdp_base.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	netdata.h \
	pdp_program.h \
	pdp_project.h \
	pdpserver.h \
	v3_compat.h \
	$(HEADS_GUI)

SRCS =\
	pdp_base.cpp \
	spec.cpp \
	netstru.cpp \
	netstru_extra.cpp \
	netdata.cpp \
	pdp_program.cpp \
	pdp_project.cpp \
	pdpserver.cpp \
	pdpserver.moc.cpp \
	$(SRCS_GUI) \
	pdp_ti.cpp \
	v3_compat.cpp


include_HEADERS = $(HEADS) $(TAHEADS) emergent_TA_type.h emergent_TA_inst.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	libemergentEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@.la
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @PDP_INCLUDES@
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	libemergentEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@.la
libemergentEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =\
	emergent_TA.cpp \
	emergent_TA_type.h \
	emergent_TA_inst.h
libemergentEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = $(libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)
libemergentEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = -g -O0

pdp_def.h:
