PROJNAME = tamisc
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

if TA_GUI

TAHEADS_GUI_PRE =\
	fontspec.h \
	xform.h

TAHEADS_GUI =\
	colorbar_qt.h \
	@abs_top_builddir@/src/taiqtso/safeptr_so.h \
	t3node_so.h \
	t3viewer.h \
	colorbar_so.h \
	datatable_qtso.h \
	datagraph_so.h \
	datagraph_qtso.h

HEADS_GUI =\
	fontspec.h \
	xform.h \
	t3node_so.h \
	t3viewer.h \
	colorbar_qt.h \
	colorbar_so.h \
	datatable_qtso.h \
	datagraph_so.h \
	datagraph_qtso.h

SRCS_GUI =\
	fontspec.cpp \
	xform.cpp \
	t3node_so.cpp \
	t3viewer.cpp \
	t3viewer.moc.cpp \
	colorbar_qt.cpp \
	colorbar_qt.moc.cpp \
	colorbar_so.cpp \
	datatable_qtso.cpp \
	datatable_qtso.moc.cpp \
	datagraph_so.cpp \
	datagraph_qtso.cpp \
	datagraph_qtso.moc.cpp

endif

TAHEADS =\
	@abs_top_builddir@/src/ta/ta_TA_type.h \
	$(TAHEADS_GUI_PRE) \
	tarandom.h \
	tdgeometry.h \
	aggregate.h \
	minmax.h \
	colorscale.h \
	datatable.h \
	datagraph.h \
	fun_lookup.h \
	$(TAHEADS_GUI) \
	tamisc_maketa_defs.h \
	tamisc_stdef.h

HEADS =\
	tamisc_TA_type.h \
	tamisc_TA_inst.h \
	tamisc_stdef.h \
	tarandom.h \
	tdgeometry.h \
	aggregate.h \
	minmax.h \
	colorscale.h \
	datatable.h \
	datagraph.h \
	fun_lookup.h \
	$(HEADS_GUI)

SRCS =\
	tarandom.cpp \
	tdgeometry.cpp \
	aggregate.cpp \
	minmax.cpp \
	colorscale.cpp \
	datatable.cpp \
	datagraph.cpp \
	fun_lookup.cpp \
	$(SRCS_GUI) \
	tamisc_TA.cpp \
	tamisc_ti.cpp

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Maketa.am
MAKETA_FLAGS += $(libtamisc_la_CPPFLAGS)
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libtamisc.la
libtamisc_la_SOURCES = $(HEADS) $(SRCS)
libtamisc_la_LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
libtamisc_la_CPPFLAGS = $(PREPROCESSOR_FLAGS) $(GUI_PREPROCESSOR_FLAGS)
libtamisc_la_CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)
BUILT_SOURCES = tamisc_TA_type.h tamisc_TA_inst.h