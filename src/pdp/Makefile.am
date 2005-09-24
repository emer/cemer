PROJNAME = pdp
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

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
	pdp_TA_type.h \
	pdp_TA_inst.h \
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
	pdp_TA.cpp \
	pdp_ti.cpp

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Maketa.am
MAKETA_FLAGS += $(libpdp_la_CPPFLAGS)
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libpdp.la
libpdp_la_SOURCES = $(HEADS) $(SRCS)
libpdp_la_LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
libpdp_la_CPPFLAGS = $(PREPROCESSOR_FLAGS) $(GUI_PREPROCESSOR_FLAGS)
libpdp_la_CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)

bin_PROGRAMS = pdp++
pdp___SOURCES = main.cpp
pdp___LDADD =\
	libpdp.la \
	@abs_top_builddir@/src/ta/libta.la \
	@abs_top_builddir@/src/css/libcss.la \
	@abs_top_builddir@/src/tamisc/libtamisc.la

if TA_GUI
pdp___LDADD += @abs_top_builddir@/src/taiqtso/libtaiqtso.la
endif

pdp___LDFLAGS = $(libpdp_la_LDFLAGS)
pdp___CPPFLAGS =$(libpdp_la_CPPFLAGS)
pdp___CFLAGS = $(libpdp_la_CFLAGS)
BUILT_SOURCES = pdp_TA_type.h pdp_TA_inst.h pdp_TA.cpp