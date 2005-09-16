PROJNAME = ta
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

if TA_GUI

ta_type_qt.cpp: ta_type.h

TAHEADS_GUI =\
	ta_qt.h \
	ta_qtdata_def.h \
	ta_qtdata.h \
	ta_qtdialog.h \
	ta_qttype_def.h \
	ta_qttype.h \
	ta_qtclipdata.h \
	ta_qtbrowse_def.h \
	ta_qtviewer.h \
	ta_qtbrowse.h \
	ta_qtgroup.h \
	ta_seledit.h \
	$(top_srcdir)/src/taiqtso/igeometry.h

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
	@abs_top_builddir@/src/ta/ios-g++-3.1/streambuf.h \
	@abs_top_builddir@/src/ta/ios-g++-3.1/iostream.h \
	@abs_top_builddir@/src/ta/ios-g++-3.1/fstream.h \
	@abs_top_builddir@/src/ta/ios-g++-3.1/sstream.h \
	ta_def.h \
	ta_string.h \
	ta_stdef.h \
	ifont.h \
	ta_list.h \
	ta_filer.h \
	icolor.h \
	ta_type.h \
	ta_base.h \
	ta_group.h \
	ta_dump.h \
	ta_defaults.h \
	ta_script.h \
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
	ta_group.cpp \
	ta_dump.cpp \
	ta_defaults.cpp \
	ta_script.cpp \
	$(SRCS_GUI) \
	ta_TA.cpp \
	ta_ti.cpp

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Maketa.am
MAKETA_FLAGS += $(libta_la_CPPFLAGS)
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libta.la
libta_la_SOURCES = $(HEADS) $(SRCS)
libta_la_LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
libta_la_CPPFLAGS = $(PREPROCESSOR_FLAGS) $(GUI_PREPROCESSOR_FLAGS) -DTA_USE_QT
libta_la_CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)
BUILT_SOURCES = $(PROJNAME)_TA_type.h $(PROJNAME)_TA_inst.h