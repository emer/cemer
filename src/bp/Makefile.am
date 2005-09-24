PROJNAME = bp
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

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

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Maketa.am

bin_PROGRAMS = bp++
bp___SOURCES = $(HEADS) $(SRCS)
bp___LDADD =\
	$(abs_top_builddir)/src/pdp/libpdp.la \
	$(abs_top_builddir)/src/tamisc/libtamisc.la \
	$(abs_top_builddir)/src/ta/libta.la \
	$(abs_top_builddir)/src/css/libcss.la


if TA_GUI
bp___LDADD += $(abs_top_builddir)/src/taiqtso/libtaiqtso.la
endif

bp___LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
bp___CPPFLAGS = $(PREPROCESSOR_FLAGS) $(GUI_PREPROCESSOR_FLAGS)
bp___CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)
BUILT_SOURCES = bp_TA_type.h bp_TA_inst.h bp_TA.cpp
