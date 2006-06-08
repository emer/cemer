#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = tamisc
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI_PRE =\
	@abs_srcdir@/fontspec.h 

TAHEADS_GUI =\
	@abs_srcdir@/colorbar_qt.h \
	@abs_srcdir@/../taiqtso/safeptr_so.h \
	@abs_srcdir@/t3node_so.h \
	@abs_srcdir@/t3viewer.h \
	@abs_srcdir@/colorbar_so.h \
	@abs_srcdir@/datatable_qtso.h \
	@abs_srcdir@/datagraph_so.h \
	@abs_srcdir@/datagraph_qtso.h

HEADS_GUI =\
	fontspec.h \
	t3node_so.h \
	t3viewer.h \
	colorbar_qt.h \
	colorbar_so.h \
	datatable_qtso.h \
	datagraph_so.h \
	datagraph_qtso.h

SRCS_GUI =\
	fontspec.cpp \
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
	@abs_srcdir@/../ta/ta_TA_type.h \
	$(TAHEADS_GUI_PRE) \
	@abs_srcdir@/tarandom.h \
	@abs_srcdir@/tdgeometry.h \
	@abs_srcdir@/aggregate.h \
	@abs_srcdir@/minmax.h \
	@abs_srcdir@/colorscale.h \
	@abs_srcdir@/ta_data.h \
	@abs_srcdir@/datatable.h \
	@abs_srcdir@/datagraph.h \
	@abs_srcdir@/fun_lookup.h \
	@abs_srcdir@/ta_data_extras.h \
	$(TAHEADS_GUI) \
	@abs_srcdir@/tamisc_maketa_defs.h \
	@abs_srcdir@/tamisc_stdef.h

HEADS =\
	tamisc_stdef.h \
	tarandom.h \
	tdgeometry.h \
	aggregate.h \
	minmax.h \
	colorscale.h \
	ta_data.h \
	datatable.h \
	datagraph.h \
	fun_lookup.h \
	ta_data_extras.h \
	$(HEADS_GUI)

SRCS =\
	tarandom.cpp \
	tdgeometry.cpp \
	aggregate.cpp \
	minmax.cpp \
	colorscale.cpp \
	ta_data.cpp \
	datatable.cpp \
	datagraph.cpp \
	fun_lookup.cpp \
	ta_data_extras.cpp \
	$(SRCS_GUI) \
	tamisc_ti.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libtamisc@PDP_SUFFIX@.la \
	libtamiscTypeAccess@PDP_SUFFIX@.la

libtamisc@PDP_SUFFIX@_la_DEPENDENCIES = libtamiscTypeAccess@PDP_SUFFIX@.la

libtamisc@PDP_SUFFIX@_la_SOURCES = $(SRCS) $(HEADS)

libtamisc@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@srcdir@/../ \
	-I@abs_top_builddir@ \
	-I@srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso

libtamisc@PDP_SUFFIX@_la_LIBADD =\
	libtamiscTypeAccess@PDP_SUFFIX@.la

libtamisc@PDP_SUFFIX@_la_CXXFLAGS = @OPT_LEVEL@

libtamiscTypeAccess@PDP_SUFFIX@_la_SOURCES = tamisc_TA.cpp tamisc_TA_type.h tamisc_TA_inst.h

libtamiscTypeAccess@PDP_SUFFIX@_la_CPPFLAGS = $(libtamisc@PDP_SUFFIX@_la_CPPFLAGS)

libtamiscTypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -g -O0
