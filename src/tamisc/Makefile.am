#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = tamisc
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI_PRE =\
	@abs_srcdir@/fontspec.h \
	@abs_srcdir@/xform.h

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
	@abs_srcdir@/../ta/ta_TA_type.h \
	$(TAHEADS_GUI_PRE) \
	@abs_srcdir@/tarandom.h \
	@abs_srcdir@/tdgeometry.h \
	@abs_srcdir@/aggregate.h \
	@abs_srcdir@/minmax.h \
	@abs_srcdir@/ta_data_defs.h \
	@abs_srcdir@/ta_data.h \
	@abs_srcdir@/colorscale.h \
	@abs_srcdir@/datatable.h \
	@abs_srcdir@/datagraph.h \
	@abs_srcdir@/fun_lookup.h \
	$(TAHEADS_GUI) \
	@abs_srcdir@/tamisc_maketa_defs.h \
	@abs_srcdir@/tamisc_stdef.h

HEADS =\
	tamisc_TA_type.h \
	tamisc_TA_inst.h \
	tamisc_stdef.h \
	tarandom.h \
	tdgeometry.h \
	aggregate.h \
	minmax.h \
	ta_data_defs.h \
	ta_data.h \
	colorscale.h \
	datatable.h \
	datagraph.h \
	-I@srcdir@/../taiqtso \
	-I/usr/include/freetype2 \
	fun_lookup.h \
	$(HEADS_GUI)

SRCS =\
	tarandom.cpp \
	tdgeometry.cpp \
	aggregate.cpp \
	minmax.cpp \
	ta_data.cpp \
	colorscale.cpp \
	datatable.cpp \
	datagraph.cpp \
	fun_lookup.cpp \
	$(SRCS_GUI) \
	tamisc_TA.cpp \
	tamisc_ti.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libtamisc@PDP_SUFFIX@.la
libtamisc@PDP_SUFFIX@_la_SOURCES = $(SRCS) $(HEADS)
libtamisc@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso \
	-I/usr/include/freetype2 \
	-I${QTDIR}/include

BUILT_SOURCES = tamisc_TA_type.h tamisc_TA_inst.h tamisc_TA.cpp

clean-local:
	-rm *TA*
	-rm *moc*
	-rm *~*
	-rm *\#*
