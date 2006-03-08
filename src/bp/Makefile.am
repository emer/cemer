#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = bp
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/../bp/bp.h \
	@abs_srcdir@/../bp/rbp.h \
	@abs_srcdir@/../bp/bp_v3_compat.h

HEADS=\
	bp.h \
	rbp.h \
	bp_v3_compat.h


SRCS=\
	bp.cpp \
	rbp.cpp  \
	bp_v3_compat.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libbp_TypeAccess@PDP_SUFFIX@.la \
	libbp@PDP_SUFFIX@.la

libbp@PDP_SUFFIX@_la_SOURCES = $(HEADS) $(SRCS)

libbp@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@srcdir@/.. \
	-I@abs_top_builddir@ \
	-I@abs_srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../pdp \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso

libbp@PDP_SUFFIX@_la_CXXFLAGS = @OPT_LEVEL@

libbp@PDP_SUFFIX@_la_LIBADD = libbp_TypeAccess@PDP_SUFFIX@.la

libbp@PDP_SUFFIX@_la_DEPENDENCIES = libbp_TypeAccess@PDP_SUFFIX@.la

libbp_TypeAccess@PDP_SUFFIX@_la_SOURCES =\
	bp_TA.cpp \
	bp_TA_type.h \
	bp_TA_inst.h

libbp_TypeAccess@PDP_SUFFIX@_la_CPPFLAGS = $(libbp@PDP_SUFFIX@_la_CPPFLAGS)

libbp_TypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -O0

BUILT_SOURCES = bp_TA_type.h

