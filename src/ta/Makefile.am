#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = ta
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_TAMISC_GUI =\
	@abs_srcdir@/colorbar_qt.h \
	@abs_srcdir@/../taiqtso/safeptr_so.h \
	@abs_srcdir@/t3node_so.h \
	@abs_srcdir@/t3viewer.h \
	@abs_srcdir@/colorbar_so.h \
	@abs_srcdir@/ta_datatable_qtso.h \
	@abs_srcdir@/ta_datatable_so.h 

HEADS_TAMISC_GUI =\
	t3node_so.h \
	t3viewer.h \
	colorbar_qt.h \
	colorbar_so.h \
	ta_datatable_qtso.h \
	ta_datatable_so.h

SRCS_TAMISC_GUI =\
	t3node_so.cpp \
	t3viewer.cpp \
	t3viewer.moc.cpp \
	colorbar_qt.cpp \
	colorbar_qt.moc.cpp \
	colorbar_so.cpp \
	ta_datatable_qtso.cpp \
	ta_datatable_qtso.moc.cpp \
	ta_datatable_so.cpp

TAHEADS_GUI =\
	@abs_srcdir@/ta_qt.h \
	@abs_srcdir@/ta_qtdata_def.h \
	@abs_srcdir@/ta_qtdata.h \
	@abs_srcdir@/ta_qtdialog.h \
	@abs_srcdir@/ta_qttype_def.h \
	@abs_srcdir@/ta_qttype.h \
	@abs_srcdir@/ta_qtviewer.h \
	@abs_srcdir@/ta_matrix_qt.h \
	@abs_srcdir@/ta_qtclassbrowse.h \
	@abs_srcdir@/ta_qtgroup.h \
	@abs_srcdir@/ta_seledit.h \
	@abs_srcdir@/ta_program_qt.h 

HEADS_GUI =\
	ta_qt.h \
	ta_qtdata_def.h \
	ta_qtdata.h \
	ta_qtdialog.h \
	ta_qttype_def.h \
	ta_qttype.h \
	ta_qtgroup.h \
	ta_qtviewer.h \
	ta_matrix_qt.h \
	ta_qtclassbrowse.h \
	ta_qtcursors.h \
	ta_seledit.h \
	ta_program_qt.h  

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
	ta_matrix_qt.cpp \
	ta_matrix_qt.moc.cpp \
	ta_qtclassbrowse.cpp \
	ta_qtclassbrowse.moc.cpp \
	ta_qttype.cpp \
	ta_seledit.cpp \
	ta_program_qt.cpp  \
	ta_program_qt.moc.cpp \
	ta_filer_qt.cpp

endif

TAHEADS_TAMISC =\
	@abs_srcdir@/ta_time.h \
	@abs_srcdir@/fun_lookup.h \
	@abs_srcdir@/ta_data_extras.h 

HEADS_TAMISC =\
	ta_time.h \
	fun_lookup.h \
	ta_data_extras.h

SRCS_TAMISC =\
	ta_time.cpp \
	fun_lookup.cpp \
	ta_data_extras.cpp 

TAHEADS =\
	@abs_srcdir@/../taiqtso/igeometry.h \
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
	@abs_srcdir@/ta_mtrnd.h \
	@abs_srcdir@/ta_list.h \
	@abs_srcdir@/ta_filer.h \
	@abs_srcdir@/icolor.h \
	@abs_srcdir@/ta_type.h \
	@abs_srcdir@/ta_base.h \
	@abs_srcdir@/ta_dmem.h \
	@abs_srcdir@/ta_fontspec.h \
	@abs_srcdir@/minmax.h \
	@abs_srcdir@/ta_group.h \
	@abs_srcdir@/ta_qtclipdata.h \
	@abs_srcdir@/colorscale.h \
	@abs_srcdir@/ta_script.h \
	@abs_srcdir@/ta_dynenum.h \
	@abs_srcdir@/ta_viewspec.h \
	@abs_srcdir@/ta_viewer.h \
	@abs_srcdir@/ta_program.h \
	@abs_srcdir@/ta_matrix.h \
	@abs_srcdir@/ta_math.h \
	@abs_srcdir@/ta_geometry.h \
	@abs_srcdir@/ta_data.h \
	@abs_srcdir@/ta_datatable.h \
	@abs_srcdir@/ta_dataproc.h \
	@abs_srcdir@/ta_dataanal.h \
	@abs_srcdir@/ta_datagen.h \
	@abs_srcdir@/ta_imgproc.h \
	@abs_srcdir@/ta_dump.h \
	@abs_srcdir@/ta_defaults.h \
	@abs_srcdir@/ta_plugin.h \
	@abs_srcdir@/ta_plugin_p.h \
	@abs_srcdir@/ta_project.h \
	$(TAHEADS_GUI) \
	$(TAHEADS_TAMISC) \
	$(TAHEADS_TAMISC_GUI)

HEADS =\
	ta_def.h \
	ta_platform.h \
	ta_thread.h \
	ta_string.h \
	ta_stdef.h \
	ta_variant.h \
	ifont.h \
	icolor.h \
	ta_mtrnd.h \
	ta_list.h \
	ta_filer.h \
	ta_type.h \
	ta_type_constr.h \
	ta_base.h \
	ta_fontspec.h \
	minmax.h \
	ta_group.h \
	ta_qtclipdata.h \
	colorscale.h \
	ta_dmem.h \
	ta_script.h \
	ta_dynenum.h \
	ta_viewspec.h \
	ta_viewer.h \
	ta_program.h \
	ta_matrix.h \
	ta_data.h \
	ta_math.h \
	ta_geometry.h \
	ta_datatable.h \
	ta_dataproc.h \
	ta_dataanal.h \
	ta_datagen.h \
	ta_imgproc.h \
	ta_dump.h \
	ta_defaults.h \
	ta_plugin.h \
	ta_plugin_p.h \
	ta_project.h \
	$(HEADS_GUI) \
	$(HEADS_TAMISC) \
	$(HEADS_TAMISC_GUI)

SRCS =\
	ta_imgproc.cpp \
	ta_platform.cpp \
	ta_thread.cpp \
	ta_string.cpp \
	ta_variant.cpp \
	ifont.cpp \
	icolor.cpp \
	ta_mtrnd.cpp \
	ta_list.cpp \
	ta_filer.cpp \
	ta_type.cpp \
	ta_type.moc.cpp \
	ta_type_constr.cpp \
	ta_base.cpp \
	ta_dmem.cpp \
	ta_fontspec.cpp \
	minmax.cpp \
	ta_group.cpp \
	ta_qtclipdata.cpp \
	ta_qtclipdata.moc.cpp \
	colorscale.cpp \
	ta_script.cpp \
	ta_dynenum.cpp \
	ta_viewspec.cpp \
	ta_viewer.cpp \
	ta_program.cpp \
	ta_matrix.cpp \
	ta_matrix.moc.cpp \
	ta_math.cpp \
	ta_geometry.cpp \
	ta_data.cpp \
	ta_datatable.cpp \
	ta_dataproc.cpp \
	ta_dataanal.cpp \
	ta_datagen.cpp \
	ta_dump.cpp \
	ta_defaults.cpp \
	ta_plugin_p.cpp \
	ta_plugin_p.moc.cpp \
	ta_project.cpp \
	ta_project.moc.cpp \
	$(SRCS_GUI) \
	ta_qrc.cpp \
	ta_ti.cpp \
	$(SRCS_TAMISC_GUI) \
	$(SRCS_TAMISC)

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libta@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	libtaTypeAccess@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES =\
	libtaTypeAccess@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(SRCS) $(HEADS)
libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @PDP_INCLUDES@
libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	libtaTypeAccess@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libtaTypeAccess@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =\
	ta_TA.cpp \
	ta_TA_type.h \
	ta_TA_inst.h

libtaTypeAccess@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS =\
	$(libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libtaTypeAccess@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = -g -O0

taglobal.h ta_maketa_defs.h qtdefs.h:
