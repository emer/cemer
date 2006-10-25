#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = tamisc
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

TAHEADS_GUI =\
	@abs_srcdir@/colorbar_qt.h \
	@abs_srcdir@/../taiqtso/safeptr_so.h \
	@abs_srcdir@/t3node_so.h \
	@abs_srcdir@/t3viewer.h \
	@abs_srcdir@/colorbar_so.h \
	@abs_srcdir@/datatable_qtso.h \
	@abs_srcdir@/datatable_so.h 

HEADS_GUI =\
	t3node_so.h \
	t3viewer.h \
	colorbar_qt.h \
	colorbar_so.h \
	datatable_qtso.h \
	datatable_so.h

SRCS_GUI =\
	t3node_so.cpp \
	t3viewer.cpp \
	t3viewer.moc.cpp \
	colorbar_qt.cpp \
	colorbar_qt.moc.cpp \
	colorbar_so.cpp \
	datatable_qtso.cpp \
	datatable_qtso.moc.cpp \
	datatable_so.cpp

endif

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/tarandom.h \
	@abs_srcdir@/tatime.h \
	@abs_srcdir@/tdgeometry.h \
	@abs_srcdir@/minmax.h \
	@abs_srcdir@/colorscale.h \
	@abs_srcdir@/datatable.h \
	@abs_srcdir@/fun_lookup.h \
	@abs_srcdir@/ta_data_extras.h \
	$(TAHEADS_GUI) \
	@abs_srcdir@/tamisc_maketa_defs.h \
	@abs_srcdir@/tamisc_stdef.h

HEADS =\
	tamisc_stdef.h \
	tarandom.h \
	tatime.h \
	tdgeometry.h \
	minmax.h \
	colorscale.h \
	datatable.h \
	fun_lookup.h \
	ta_data_extras.h \
	$(HEADS_GUI)

SRCS =\
	tarandom.cpp \
	tatime.cpp \
	tdgeometry.cpp \
	minmax.cpp \
	colorscale.cpp \
	datatable.cpp \
	fun_lookup.cpp \
	ta_data_extras.cpp \
	$(SRCS_GUI) \
	tamisc_ti.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libtamisc@PDP_SUFFIX@@PDP_VERSION@.la \
	libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_DEPENDENCIES = libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = $(SRCS) $(HEADS)

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS =\
	-I@srcdir@/../ \
	-I@abs_top_builddir@ \
	-I@srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD =\
	libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@.la

libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = tamisc_TA.cpp tamisc_TA_type.h tamisc_TA_inst.h

libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS = $(libtamisc@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS)

libtamiscTypeAccess@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = -g -O0
