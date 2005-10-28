#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = leabra
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/leabra.h
HEADS =\
	leabra_TA_type.h \
	leabra_TA_inst.h \
	leabra.h
SRCS =\
	leabra.cpp \
	leabra_TA.cpp \
	main.cpp

include $(top_srcdir)/Maketa.am

bin_PROGRAMS = leabra@PDP_SUFFIX@++
leabra@PDP_SUFFIX@___SOURCES = $(HEADS) $(SRCS)
leabra@PDP_SUFFIX@___LDADD =\
	$(abs_top_builddir)/src/pdp/libpdp@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/ta/libta@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/css/libcss@PDP_SUFFIX@.la
leabra@PDP_SUFFIX@___CPPFLAGS =\
	-I@srcdir@/../ta \
	-I@srcdir@/../pdp \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso \
	-I/usr/include/freetype2
BUILT_SOURCES = leabra_TA_type.h
#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI
leabra@PDP_SUFFIX@___LDADD += $(abs_top_builddir)/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la
endif
