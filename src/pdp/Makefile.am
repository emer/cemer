#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = pdp
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI =\
	@abs_srcdir@/pdpview_qt.h \
	@abs_srcdir@/netstru_so.h \
	@abs_srcdir@/netstru_qtso.h \
	@abs_srcdir@/pdp_qtso.h \
	@abs_srcdir@/pdplog_qtso.h \
	@abs_srcdir@/enviro_qtso.h

HEADS_GUI =\
	pdpview_qt.h \
	netstru_so.h \
	netstru_qtso.h \
	pdp_qtso.h \
	pdplog_qtso.h \
	enviro_qtso.h

SRCS_GUI =\
	pdpview_qt.cpp \
	netstru_so.cpp \
	pdp_qtso.cpp \
	pdp_qtso.moc.cpp \
	netstru_qtso.cpp \
	netstru_qtso.moc.cpp \
	pdplog_qtso.cpp \
	pdplog_qtso.moc.cpp \
	enviro_qtso.cpp

endif

ta_TA_type.h tamisc_TA_type.h:
TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/pdpdeclare.h \
	@abs_srcdir@/pdpbase.h \
	@abs_srcdir@/spec.h \
	@abs_srcdir@/netstru.h \
	@abs_srcdir@/netstru_extra.h \
	@abs_srcdir@/enviro.h \
	@abs_srcdir@/enviro_extra.h \
	@abs_srcdir@/process.h \
	@abs_srcdir@/stats.h \
	@abs_srcdir@/pdplog.h \
	@abs_srcdir@/sched_proc.h \
	@abs_srcdir@/procs_extra.h \
	@abs_srcdir@/pdpshell.h \
	$(TAHEADS_GUI)

HEADS =\
	pdpdeclare.h \
	pdpbase.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	enviro.h \
	enviro_extra.h \
	process.h \
	stats.h \
	sched_proc.h \
	procs_extra.h \
	pdplog.h \
	pdpshell.h \
	$(HEADS_GUI)

SRCS =\
	pdpbase.cpp \
	pdpbase_unix.cpp \
	spec.cpp \
	netstru.cpp \
	netstru_extra.cpp \
	enviro.cpp \
	enviro_extra.cpp \
	process.cpp \
	stats.cpp \
	sched_proc.cpp \
	procs_extra.cpp \
	pdplog.cpp \
	pdpshell.cpp \
	$(SRCS_GUI) \
	pdp_ti.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libpdp@PDP_SUFFIX@.la
libpdp@PDP_SUFFIX@_la_DEPENDENCIES = libpdpTypeAccess@PDP_SUFFIX@.la
libpdp@PDP_SUFFIX@_la_SOURCES = $(HEADS) $(SRCS)
libpdp@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@abs_top_builddir@ \
	-I@srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../taiqtso
libpdp@PDP_SUFFIX@_la_CXXFLAGS = -O2
libpdp@PDP_SUFFIX@_la_LIBADD = libpdpTypeAccess@PDP_SUFFIX@.la

noinst_LTLIBRARIES = libpdpTypeAccess@PDP_SUFFIX@.la
libpdpTypeAccess@PDP_SUFFIX@_la_SOURCES = pdp_TA.cpp pdp_TA_type.h pdp_TA_inst.h
libpdpTypeAccess@PDP_SUFFIX@_la_CPPFLAGS = $(libpdp@PDP_SUFFIX@_la_CPPFLAGS)
libpdpTypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -g -O0

bin_PROGRAMS = pdp@PDP_SUFFIX@++
pdp@PDP_SUFFIX@___DEPENDENCIES = libpdp@PDP_SUFFIX@.la
pdp@PDP_SUFFIX@___SOURCES = main.cpp
pdp@PDP_SUFFIX@___LDADD =\
	@abs_top_builddir@/src/pdp/libpdp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/tamisc/libtamisc@PDP_SUFFIX@.la
pdp@PDP_SUFFIX@___CPPFLAGS = $(libpdp@PDP_SUFFIX@_la_CPPFLAGS)
pdp@PDP_SUFFIX@___CXXFLAGS = $(libpdp@PDP_SUFFIX@_la_CXXFLAGS)

BUILT_SOURCES = pdp_TA_type.h
CLEANFILES += pdp_TA.ccx

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI
pdp@PDP_SUFFIX@___LDADD += @abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la
endif

clean-local:
	-rm *moc*
