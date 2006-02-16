#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = bp
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../tamisc/tamisc_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/../bp/bp.h \
	@abs_srcdir@/../bp/rbp.h \
	@abs_srcdir@/../bp/bp_v3_compat.h

HEADS =\
	bp.h \
	rbp.h \
	bp_v3_compat.h

SRCS =\
	bp.cpp \
	rbp.cpp  \
	bp_v3_compat.cpp \
	main.cpp

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libbp_TypeAccess@PDP_SUFFIX@.la

libbp_TypeAccess@PDP_SUFFIX@_la_SOURCES = bp_TA.cpp bp_TA_type.h bp_TA_inst.h

libbp_TypeAccess@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@srcdir@/../ \
	-I@abs_top_builddir@ \
	-I@abs_srcdir@ \
	-I@srcdir@/../ta \
	-I@srcdir@/../pdp \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso

libbp_TypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -O0

bin_PROGRAMS = bp@PDP_SUFFIX@++

bp@PDP_SUFFIX@___DEPENDENCIES = libbp_TypeAccess@PDP_SUFFIX@.la

bp@PDP_SUFFIX@___SOURCES = $(HEADS) $(SRCS)

bp@PDP_SUFFIX@___LDADD =\
	$(abs_top_builddir)/src/bp/libbp_TypeAccess@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/pdp/libpdp@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/ta/libta@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/css/libcss@PDP_SUFFIX@.la

bp@PDP_SUFFIX@___CPPFLAGS = $(libbp_TypeAccess@PDP_SUFFIX@_la_CPPFLAGS)

bp@PDP_SUFFIX@___CXXFLAGS = @OPT_LEVEL@

BUILT_SOURCES = bp_TA_type.hx

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI

bp@PDP_SUFFIX@___LDADD += $(abs_top_builddir)/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la

endif
