PROJNAME = ta
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI =\
	@abs_srcdir@/ta_qt.h \
	@abs_srcdir@/ta_qtdata_def.h \
	@abs_srcdir@/ta_qtdata.h \
	@abs_srcdir@/ta_qtdialog.h \
	@abs_srcdir@/ta_qttype_def.h \
	@abs_srcdir@/ta_qttype.h \
	@abs_srcdir@/ta_qtclipdata.h \
	@abs_srcdir@/ta_qtbrowse_def.h \
	@abs_srcdir@/ta_qtviewer.h \
	@abs_srcdir@/ta_qtbrowse.h \
	@abs_srcdir@/ta_qtgroup.h \
	@abs_srcdir@/ta_seledit.h \
	@abs_srcdir@/../taiqtso/igeometry.h

HEADS_GUI =\
	ta_qt.h \
	ta_qtdata_def.h \
	ta_qtdata.h \
	ta_qtdialog.h \
	ta_qttype_def.h \
	ta_qttype.h \
	ta_qtbrowse_def.h \
	ta_qtgroup.h \
	ta_qtviewer.h \
	ta_qtbrowse.h \
	ta_qtcursors.h \
	ta_qtclipdata.h \
	ta_seledit.h

SRCS_GUI =\
	ta_qt.cpp \
	ta_qt.moc.cpp \
	ta_type_qt.cpp \
	ta_qtdata.cpp \
	ta_qtdata_def.moc.cpp \
	ta_qtdata.moc.cpp \
	ta_qtdialog.cpp \
	ta_qtdialog.moc.cpp \
	ta_qtgroup.cpp \
	ta_qtgroup.moc.cpp \
	ta_qtviewer.cpp \
	ta_qtviewer.moc.cpp \
	ta_qtbrowse.cpp \
	ta_qtbrowse.moc.cpp \
	ta_qttype.cpp \
	ta_qtclipdata.cpp \
	ta_qtclipdata.moc.cpp \
	ta_seledit.cpp

endif

TAHEADS =\
	@abs_srcdir@/ios-g++-3.1/streambuf.h \
	@abs_srcdir@/ios-g++-3.1/iostream.h \
	@abs_srcdir@/ios-g++-3.1/fstream.h \
	@abs_srcdir@/ios-g++-3.1/sstream.h \
	@abs_srcdir@/ta_def.h \
	@abs_srcdir@/ta_string.h \
	@abs_srcdir@/ta_stdef.h \
	@abs_srcdir@/ifont.h \
	@abs_srcdir@/ta_list.h \
	@abs_srcdir@/ta_filer.h \
	@abs_srcdir@/icolor.h \
	@abs_srcdir@/ta_type.h \
	@abs_srcdir@/ta_base.h \
	@abs_srcdir@/ta_matrix.h \
	@abs_srcdir@/ta_group.h \
	@abs_srcdir@/ta_dump.h \
	@abs_srcdir@/ta_defaults.h \
	@abs_srcdir@/ta_script.h \
	$(TAHEADS_GUI)

HEADS =\
	ta_def.h \
	$(PROJNAME)_TA_type.h \
	$(PROJNAME)_TA_inst.h \
	ta_platform.h \
	ta_string.h \
	ta_stdef.h \
	ifont.h \
	ta_list.h \
	ta_filer.h \
	icolor.h \
	ta_type.h \
	ta_type_constr.h \
	ta_base.h \
	ta_matrix.h \
	ta_group.h \
	ta_dump.h \
	ta_defaults.h \
	ta_script.h \
	$(HEADS_GUI)

SRCS =\
	ta_platform.cpp \
	ta_platform_unix.cpp \
	ta_string.cpp \
	ifont.cpp \
	ta_list.cpp \
	ta_filer.cpp \
	icolor.cpp \
	ta_type.cpp \
	ta_type_constr.cpp \
	ta_base.cpp \
	ta_matrix.cpp \
	ta_group.cpp \
	ta_dump.cpp \
	ta_defaults.cpp \
	ta_script.cpp \
	$(SRCS_GUI) \
	ta_TA.cpp \
	ta_ti.cpp

include $(top_srcdir)/Maketa.am
MAKETA_FLAGS += $(libta@PDP_SUFFIX@_la_CPPFLAGS)
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libta@PDP_SUFFIX@.la
libta@PDP_SUFFIX@_la_SOURCES = $(SRCS) $(HEADS)

libta@PDP_SUFFIX@_la_CPPFLAGS = -I@srcdir@/../css

libta@PDP_SUFFIX@_la_CXXFLAGS =

AM_CXXFLAGS = -fno-implicit-templates

BUILT_SOURCES = ta_TA_type.h ta_TA_inst.h ta_TA.cpp

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI

libta@PDP_SUFFIX@_la_LIBADD =\
	@X_EXTRA_LIBS@ \
	@X_PRE_LIBS@ \
	@QT_LDADD@

libta@PDP_SUFFIX@_la_LDFLAGS = @X_LIBS@

libta@PDP_SUFFIX@_la_CPPFLAGS +=\
	-I@srcdir@/../taiqtso \
	@X_CFLAGS@ \
	-I/usr/include/freetype2 \
	@QT_CXXFLAGS@ \
	-DTA_GUI \
	-DTA_USE_QT \
	-DTA_USE_INVENTOR \
	-DQT_SHARED

libta@PDP_SUFFIX@_la_LDFLAGS += @X_LIBS@

endif
#**************************************************
#
#                      NOGUI
#
#**************************************************
if NO_TA_GUI

libta@PDP_SUFFIX@_la_CPPFLAGS += -DTA_NO_GUI


endif
#**************************************************
#
#                      DEBUG
#
#**************************************************
if DEBUG

libta@PDP_SUFFIX@_la_CPPFLAGS +=\
	-DYYDEBUG \
	-DDEBUG \
	-DTA_PROFILE \
	-DHAVE_POSIX_SIGNALS=1 \
	-DRETSIGTYPE=void

libta@PDP_SUFFIX@_la_CXXFLAGS += -Wno-non-virtual-dtor

endif
#**************************************************
#
#                      MPI
#
#**************************************************
if MPI

libta@PDP_SUFFIX@_la_CPPFLAGS += -DDMEM_COMPILE

endif
#**************************************************
#
#                      LINUX
#
#**************************************************
if LINUX

libta@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX

endif
#**************************************************
#
#                      DARWIN
#
#**************************************************
if DARWIN

libta@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX -DDARWIN

endif

clean-local:
	-rm *TA*
	-rm *moc*
	-rm *~
