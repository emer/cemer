#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = pdp
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI =\
	@abs_srcdir@/pdpview_qt.h \
	@abs_srcdir@/pdpserver_qt.h \
	@abs_srcdir@/netstru_so.h \
	@abs_srcdir@/netstru_qtso.h \
	@abs_srcdir@/pdp_qtso.h \
	@abs_srcdir@/pdplog_qtso.h \
	@abs_srcdir@/program_qtso.h 

HEADS_GUI =\
	pdpview_qt.h \
	pdpserver_qt.h \
	netstru_so.h \
	netstru_qtso.h \
	pdp_qtso.h \
	pdplog_qtso.h \
	program_qtso.h 

SRCS_GUI =\
	pdpview_qt.cpp \
	pdpserver_qt.cpp \
	pdpserver_qt.moc.cpp \
	netstru_so.cpp \
	pdp_qtso.cpp \
	pdp_qtso.moc.cpp \
	netstru_qtso.cpp \
	netstru_qtso.moc.cpp \
	pdplog_qtso.cpp \
	pdplog_qtso.moc.cpp \
	program_qtso.cpp  \
	program_qtso.moc.cpp 

endif

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/pdpdeclare.h \
	@abs_srcdir@/pdpbase.h \
	@abs_srcdir@/spec.h \
	@abs_srcdir@/netstru.h \
	@abs_srcdir@/netstru_extra.h \
	@abs_srcdir@/netdata.h \
	@abs_srcdir@/program.h \
	@abs_srcdir@/pdplog.h \
	@abs_srcdir@/pdpshell.h \
	@abs_srcdir@/pdpserver.h \
	$(TAHEADS_GUI) \
	@abs_srcdir@/v3_compat.h

HEADS =\
	pdpdeclare.h \
	pdpbase.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	netdata.h \
	program.h \
	pdplog.h \
	pdpshell.h \
	pdpserver.h \
	v3_compat.h \
	$(HEADS_GUI)

SRCS =\
	pdpbase.cpp \
	pdpbase_unix.cpp \
	spec.cpp \
	netstru.cpp \
	netstru_extra.cpp \
	netdata.cpp \
	program.cpp \
	pdplog.cpp \
	pdpshell.cpp \
	pdpserver.cpp \
	pdpserver.moc.cpp \
	$(SRCS_GUI) \
	pdp_ti.cpp \
	v3_compat.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libpdp@PDP_SUFFIX@@PDP_VERSION@.la \
	libpdpTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libpdp@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libpdp@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS =\
	-I@srcdir@/../ \
	-I@abs_top_builddir@ \
	-I@srcdir@ \
	-I@srcdir@/../bp \
	-I@srcdir@/../leabra \
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../taiqtso

libpdp@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libpdp@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD =\
	libpdpTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libpdpTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES =\
	pdp_TA.cpp \
	pdp_TA_type.h \
	pdp_TA_inst.h

libpdpTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS = $(libpdp@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS)

libpdpTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = -g -O0
