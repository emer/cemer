#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = emergent
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI =\
	netstru_so.h \
	netstru_qtso.h \
	emergent_qtso.h \
	emergent_program_qtso.h 

HEADS_GUI =\
	netstru_so.h \
	netstru_qtso.h \
	emergent_qtso.h \
	emergent_program_qtso.h 

SRCS_GUI =\
	netstru_so.cpp \
	emergent_qtso.cpp \
	netstru_qtso.cpp \
	netstru_qtso.moc.cpp \
	emergent_program_qtso.cpp

endif

TAHEADS =\
	../../temt/ta/ta_TA_type.h \
	emergent_base.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	netdata.h \
	emergent_program.h \
	emergent_project.h \
	$(TAHEADS_GUI) \
	v3_compat.h

HEADS =\
	emergent_base.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	netdata.h \
	emergent_program.h \
	emergent_project.h \
	v3_compat.h \
	$(HEADS_GUI)

SRCS =\
	emergent_base.cpp \
	spec.cpp \
	netstru.cpp \
	netstru_extra.cpp \
	netdata.cpp \
	emergent_program.cpp \
	emergent_project.cpp \
	$(SRCS_GUI) \
	emergent_ti.cpp \
	v3_compat.cpp


include_HEADERS = $(HEADS) $(TAHEADS) emergent_TA_type.h emergent_TA_inst.h emergent_def.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la \
	libemergentEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)
libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	libemergentEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
libemergentEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =\
	emergent_TA.cpp \
	emergent_TA_type.h \
	emergent_TA_inst.h
libemergentEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = $(libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)
libemergentEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = -g -O0

clean-local:
	-rm -f libemergent*
	if [ -n "`ls *\.moc\.cpp`" ];then rm `ls *\.moc\.cpp`;fi

emergent_def.h:
