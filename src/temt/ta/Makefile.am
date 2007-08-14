#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = ta
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_TAMISC_GUI =\
	colorbar_qt.h \
	../taiqtso/safeptr_so.h \
	t3node_so.h \
	SoCapsule.h \
	t3viewer.h \
	colorbar_so.h \
	ta_datatable_qtso.h \
	ta_datatable_so.h \
	ta_virtenv_qtso.h \
	ta_virtenv_so.h

HEADS_TAMISC_GUI =\
	t3node_so.h \
	SoCapsule.h \
	t3viewer.h \
	colorbar_qt.h \
	colorbar_so.h \
	ta_datatable_qtso.h \
	ta_datatable_so.h \
	ta_virtenv_qtso.h \
	ta_virtenv_so.h

SRCS_TAMISC_GUI =\
	t3node_so.cpp \
	SoCapsule.cpp \
	t3viewer.cpp \
	t3viewer.moc.cpp \
	colorbar_qt.cpp \
	colorbar_qt.moc.cpp \
	colorbar_so.cpp \
	ta_datatable_qtso.cpp \
	ta_datatable_qtso.moc.cpp \
	ta_datatable_so.cpp \
	ta_virtenv_qtso.cpp \
	ta_virtenv_qtso.moc.cpp \
	ta_virtenv_so.cpp

TAHEADS_GUI =\
	ta_qt.h \
	ta_qtdata_def.h \
	ta_qtdata.h \
	ta_qtdialog.h \
	ta_qttype_def.h \
	ta_qttype.h \
	ta_qtviewer.h \
	ta_matrix_qt.h \
	ta_qtclassbrowse.h \
	ta_qtgroup.h \
	ta_seledit.h \
	ta_program_qt.h 

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
	ta_time.h \
	fun_lookup.h \
	ta_data_extras.h 

HEADS_TAMISC =\
	ta_time.h \
	fun_lookup.h \
	ta_data_extras.h

SRCS_TAMISC =\
	ta_time.cpp \
	fun_lookup.cpp \
	ta_data_extras.cpp 

TAHEADS =\
	../taiqtso/igeometry.h \
	ios-g++-3.1/streambuf.h \
	ios-g++-3.1/iostream.h \
	ios-g++-3.1/fstream.h \
	ios-g++-3.1/sstream.h \
	ta_def.h \
	ta_platform.h \
	ta_thread.h \
	ta_string.h \
	ta_stdef.h \
	ta_variant.h \
	ifont.h \
	ta_mtrnd.h \
	ta_list.h \
	ta_filer.h \
	icolor.h \
	ta_type.h \
	ta_base.h \
	ta_dmem.h \
	ta_fontspec.h \
	minmax.h \
	ta_group.h \
	ta_qtclipdata.h \
	colorscale.h \
	ta_script.h \
	ta_viewspec.h \
	ta_viewer.h \
	ta_program.h \
	ta_program_els.h \
	ta_matrix.h \
	ta_math.h \
	ta_geometry.h \
	ta_data.h \
	ta_datatable.h \
	ta_dataproc.h \
	ta_dataanal.h \
	ta_datagen.h \
	ta_imgproc.h \
	ta_virtenv.h \
	ta_dump.h \
	ta_defaults.h \
	ta_plugin.h \
	ta_plugin_p.h \
	ta_project.h \
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
	ta_viewspec.h \
	ta_viewer.h \
	ta_program.h \
	ta_program_els.h \
	ta_matrix.h \
	ta_data.h \
	ta_math.h \
	ta_geometry.h \
	ta_datatable.h \
	ta_dataproc.h \
	ta_dataanal.h \
	ta_datagen.h \
	ta_imgproc.h \
	ta_virtenv.h \
	ta_dump.h \
	ta_defaults.h \
	ta_plugin.h \
	ta_plugin_p.h \
	ta_project.h \
	ta_gendoc.h \
	$(HEADS_GUI) \
	$(HEADS_TAMISC) \
	$(HEADS_TAMISC_GUI)

SRCS =\
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
	ta_viewspec.cpp \
	ta_viewer.cpp \
	ta_program.cpp \
	ta_program_els.cpp \
	ta_matrix.cpp \
	ta_math.cpp \
	ta_geometry.cpp \
	ta_data.cpp \
	ta_datatable.cpp \
	ta_dataproc.cpp \
	ta_dataanal.cpp \
	ta_datagen.cpp \
	ta_imgproc.cpp \
	ta_virtenv.cpp \
	ta_dump.cpp \
	ta_defaults.cpp \
	ta_plugin_p.cpp \
	ta_plugin_p.moc.cpp \
	ta_project.cpp \
	ta_project.moc.cpp \
	ta_gendoc.cpp \
	$(SRCS_GUI) \
	ta_qrc.cpp \
	ta_ti.cpp \
	$(SRCS_TAMISC_GUI) \
	$(SRCS_TAMISC)

include_HEADERS = $(HEADS) $(TAHEADS) ta_TA_type.h ta_TA_inst.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

interceptor.moc.cpp:
	$(QT_MOC) -o irenderarea.moc.cpp -I@abs_top_builddir@ -I@abs_top_builddir@/src/temt/ta interceptor.h

noinst_LTLIBRARIES =\
	libta@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	libtaEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES =\
	libtaEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(SRCS) $(HEADS)
libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @PDP_INCLUDES@
libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	libtaEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libtaEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =\
	ta_TA.cpp \
	ta_TA_type.h \
	ta_TA_inst.h

libtaEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS =\
	$(libta@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libtaEmergentToolkit@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = -g -O0

taglobal.h ta_maketa_defs.h qtdefs.h:
