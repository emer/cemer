PROJNAME = bp
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

TAHEADS =\
	$(abs_top_builddir)/src/ta/ta_TA_type.h \
	$(abs_top_builddir)/src/tamisc/tamisc_TA_type.h \
	$(abs_top_builddir)/src/pdp/pdp_TA_type.h \
	$(abs_top_builddir)/src/bp/bp.h \
	$(abs_top_builddir)/src/bp/rbp.h

HEADS =\
	bp_TA_type.h \
	bp_TA_inst.h \
	bp.h \
	rbp.h

SRCS =\
	bp.cpp \
	rbp.cc \
	bp_TA.cpp \
	main.cpp

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Maketa.am

bin_PROGRAMS = bp
bp_SOURCES = $(HEADS) $(SRCS)
bp_LDADD =\
	$(abs_top_builddir)/src/pdp/libpdp.la \
	$(abs_top_builddir)/src/tamisc/libtamisc.la \
	$(abs_top_builddir)/src/ta/libta.la \
	$(abs_top_builddir)/src/css/libcss.la


if TA_GUI
bp_LDADD += $(abs_top_builddir)/src/taiqtso/libtaiqtso.la
fi

bp_LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
bp_CPPFLAGS = $(PREPROCESSOR_FLAGS) $(GUI_PREPROCESSOR_FLAGS)
bp_CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)
BUILT_SOURCES = bp_TA_type.h bp_TA_inst.h bp_TA.cpp
