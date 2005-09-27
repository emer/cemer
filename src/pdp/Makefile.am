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

include $(top_srcdir)/Maketa.am
MAKETA_FLAGS += $(libpdp@PDP_SUFFIX@_la_CPPFLAGS)
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libpdp@PDP_SUFFIX@.la
libpdp@PDP_SUFFIX@_la_SOURCES = $(HEADS) $(SRCS)

libpdp@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../tamisc

libpdp@PDP_SUFFIX@_la_CXXFLAGS =
libpdp@PDP_SUFFIX@_la_LDFLAGS =

bin_PROGRAMS = pdp@PDP_SUFFIX@++
pdp@PDP_SUFFIX@___SOURCES = main.cpp
pdp@PDP_SUFFIX@___LDADD =\
	libpdp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/tamisc/libtamisc@PDP_SUFFIX@.la

pdp@PDP_SUFFIX@___LDFLAGS = $(libpdp@PDP_SUFFIX@_la_LDFLAGS)
pdp@PDP_SUFFIX@___CPPFLAGS = $(libpdp@PDP_SUFFIX@_la_CPPFLAGS)
pdp@PDP_SUFFIX@___CFLAGS = $(libpdp@PDP_SUFFIX@_la_CFLAGS)

BUILT_SOURCES = pdp_TA_type.h pdp_TA_inst.h pdp_TA.cpp

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI

pdp@PDP_SUFFIX@___LDADD += @abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la

libpdp@PDP_SUFFIX@_la_LIBADD =\
	@X_EXTRA_LIBS@ \
	@X_PRE_LIBS@ \
	@QT_LDADD@
	
libpdp@PDP_SUFFIX@_la_LDFLAGS += @X_LIBS@

libpdp@PDP_SUFFIX@_la_CPPFLAGS +=\
	-I../src/taiqtso \
	-I@srcdir@/../taiqtso \
	@X_CFLAGS@ \
	-I/usr/include/freetype2 \
	@QT_CXXFLAGS@ \
	-DTA_GUI \
	-DTA_USE_QT \
	-DTA_USE_INVENTOR \
	-DQT_SHARED

endif
#**************************************************
#
#                      NOGUI
#
#**************************************************
if NO_TA_GUI

libpdp@PDP_SUFFIX@_la_CPPFLAGS += -DTA_NO_GUI

endif
#**************************************************
#
#                      DEBUG
#
#**************************************************
if DEBUG

libpdp@PDP_SUFFIX@_la_CPPFLAGS +=\
	-DYYDEBUG \
	-DDEBUG \
	-DTA_PROFILE \
	-DHAVE_POSIX_SIGNALS=1 \
	-DRETSIGTYPE=void

libpdp@PDP_SUFFIX@_la_CXXFLAGS += -Wno-non-virtual-dtor

endif
#**************************************************
#
#                      MPI
#
#**************************************************
if MPI

libpdp@PDP_SUFFIX@_la_CPPFLAGS += -DDMEM_COMPILE
libpdp@PDP_SUFFIX@_la_LIBADD = @MPILIBS@

endif
#**************************************************
#
#                      LINUX
#
#**************************************************
if LINUX

libpdp@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX

endif
#**************************************************
#
#                      DARWIN
#
#**************************************************
if DARWIN

libpdp@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX -DDARWIN

endif

clean-local:
	-rm *TA*
	-rm *moc*
	-rm *~
