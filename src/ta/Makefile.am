#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = ta
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI =\
	@abs_srcdir@/ta_plugin.h \
	@abs_srcdir@/ta_qt.h \
	@abs_srcdir@/ta_qtdata_def.h \
	@abs_srcdir@/ta_qtdata.h \
	@abs_srcdir@/ta_qtdialog.h \
	@abs_srcdir@/ta_qttype_def.h \
	@abs_srcdir@/ta_qttype.h \
	@abs_srcdir@/ta_qtclipdata.h \
	@abs_srcdir@/ta_qtbrowse_def.h \
	@abs_srcdir@/ta_qtviewer.h \
	@abs_srcdir@/ta_matrix_qt.h \
	@abs_srcdir@/ta_qtbrowse.h \
	@abs_srcdir@/ta_classbrowse.h \
	@abs_srcdir@/ta_qtgroup.h \
	@abs_srcdir@/ta_seledit.h \
	@abs_srcdir@/program_qt.h \
	@abs_srcdir@/../taiqtso/igeometry.h

HEADS_GUI =\
	ta_plugin.h \
	ta_qt.h \
	ta_qtdata_def.h \
	ta_qtdata.h \
	ta_qtdialog.h \
	ta_qttype_def.h \
	ta_qttype.h \
	ta_qtbrowse_def.h \
	ta_qtgroup.h \
	ta_qtviewer.h \
	ta_matrix_qt.h \
	ta_qtbrowse.h \
	ta_classbrowse.h \
	ta_qtcursors.h \
	ta_qtclipdata.h \
	ta_seledit.h \
	program_qt.h  

SRCS_GUI =\
	ta_plugin.cpp \
	ta_plugin.moc.cpp \
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
	ta_matrix_qt.cpp \
	ta_matrix_qt.moc.cpp \
	ta_qtbrowse.cpp \
	ta_qtbrowse.moc.cpp \
	ta_classbrowse.cpp \
	ta_classbrowse.moc.cpp \
	ta_qttype.cpp \
	ta_qtclipdata.cpp \
	ta_qtclipdata.moc.cpp \
	ta_seledit.cpp \
	program_qt.cpp  \
	program_qt.moc.cpp 

endif

TAHEADS =\
	@abs_srcdir@/ios-g++-3.1/streambuf.h \
	@abs_srcdir@/ios-g++-3.1/iostream.h \
	@abs_srcdir@/ios-g++-3.1/fstream.h \
	@abs_srcdir@/ios-g++-3.1/sstream.h \
	@abs_srcdir@/ta_def.h \
	@abs_srcdir@/ta_platform.h \
	@abs_srcdir@/ta_thread.h \
	@abs_srcdir@/ta_string.h \
	@abs_srcdir@/ta_stdef.h \
	@abs_srcdir@/ta_variant.h \
	@abs_srcdir@/ifont.h \
	@abs_srcdir@/ta_list.h \
	@abs_srcdir@/ta_filer.h \
	@abs_srcdir@/icolor.h \
	@abs_srcdir@/ta_type.h \
	@abs_srcdir@/ta_base.h \
	@abs_srcdir@/ta_group.h \
	@abs_srcdir@/ta_dmem.h \
	@abs_srcdir@/ta_viewer.h \
	@abs_srcdir@/ta_matrix.h \
	@abs_srcdir@/ta_dump.h \
	@abs_srcdir@/ta_defaults.h \
	@abs_srcdir@/ta_script.h \
	@abs_srcdir@/dynenum.h \
	@abs_srcdir@/program.h \
	@abs_srcdir@/ta_project.h \
	$(TAHEADS_GUI)

HEADS =\
	ta_def.h \
	ta_platform.h \
	ta_thread.h \
	ta_string.h \
	ta_stdef.h \
	ta_variant.h \
	ifont.h \
	ta_list.h \
	ta_filer.h \
	icolor.h \
	ta_type.h \
	ta_type_constr.h \
	ta_base.h \
	ta_group.h \
	ta_dmem.h \
	ta_viewer.h \
	ta_matrix.h \
	ta_dump.h \
	ta_defaults.h \
	ta_script.h \
	dynenum.h \
	program.h \
	ta_project.h \
	$(HEADS_GUI)

SRCS =\
	ta_platform.cpp \
	ta_platform_unix.cpp \
	ta_thread.cpp \
	ta_string.cpp \
	ta_variant.cpp \
	ifont.cpp \
	ta_list.cpp \
	ta_filer.cpp \
	icolor.cpp \
	ta_type.cpp \
	ta_type.moc.cpp \
	ta_type_constr.cpp \
	ta_base.cpp \
	ta_group.cpp \
	ta_dmem.cpp \
	ta_viewer.cpp \
	ta_matrix.cpp \
	ta_dump.cpp \
	ta_defaults.cpp \
	ta_script.cpp \
	dynenum.cpp \
	program.cpp \
	ta_project.cpp \
	$(SRCS_GUI) \
	ta_qrc.cpp \
	ta_ti.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libta@PDP_SUFFIX@@PDP_VERSION@.la \
	libtaTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libta@PDP_SUFFIX@@PDP_VERSION@_la_DEPENDENCIES =\
	libtaTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libta@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = $(SRCS) $(HEADS)

libta@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS =\
	-I@srcdir@/../ \
	-I@abs_top_builddir@ \
	-I@srcdir@ \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso

libta@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libta@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD =\
	libtaTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libtaTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES =\
	ta_TA.cpp \
	ta_TA_type.h \
	ta_TA_inst.h

libtaTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS =\
	$(libta@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS)

libtaTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = -g -O0
