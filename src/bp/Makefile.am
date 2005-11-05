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
	@abs_srcdir@/../bp/rbp.h

HEADS =\
	bp_TA_type.h \
	bp_TA_inst.h \
	bp.h \
	rbp.h

SRCS =\
	bp.cpp \
	rbp.cpp \
	bp_TA.cpp \
	main.cpp

include $(top_srcdir)/Maketa.am

bin_PROGRAMS = bp@PDP_SUFFIX@++

bp@PDP_SUFFIX@___SOURCES = $(HEADS) $(SRCS)

bp@PDP_SUFFIX@___LDADD =\
	$(abs_top_builddir)/src/pdp/libpdp@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/ta/libta@PDP_SUFFIX@.la \
	$(abs_top_builddir)/src/css/libcss@PDP_SUFFIX@.la

bp@PDP_SUFFIX@___CPPFLAGS =\
	-I@srcdir@/../ta \
	-I@srcdir@/../pdp \
	-I@srcdir@/../tamisc \
	-I@srcdir@/../css \
	-I@srcdir@/../taiqtso \
	-I${QTDIR}/include

BUILT_SOURCES = bp_TA_type.h
CLEANFILES += bp_TA.ccx
#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI
bp@PDP_SUFFIX@___LDADD += $(abs_top_builddir)/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la
endif
