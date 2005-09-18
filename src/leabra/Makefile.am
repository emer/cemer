PROJNAME = leabra
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

TAHEADS =\
	$(abs_top_builddir)/src/ta/ta_TA_type.h \
	$(abs_top_builddir)/src/tamisc/tamisc_TA_type.h \
	$(abs_top_builddir)/src/pdp/pdp_TA_type.h \
	$(abs_top_builddir)/src/leabra/leabra.h

HEADS =\
	leabra_TA_type.h \
	leabra_TA_inst.h \
	leabra.h

SRCS =\
	leabra.cpp \
	leabra_TA.cpp \
	main.cpp

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Maketa.am

bin_PROGRAMS = leabra++
leabra_SOURCES = $(HEADS) $(SRCS)
leabra_LDADD =\
	$(abs_top_builddir)/src/pdp/libpdp.la \
	$(abs_top_builddir)/src/tamisc/libtamisc.la \
	$(abs_top_builddir)/src/ta/libta.la \
	$(abs_top_builddir)/src/css/libcss.la


if TA_GUI
leabra_LDADD += $(abs_top_builddir)/src/taiqtso/libtaiqtso.la
fi

leabra_LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
leabra_CPPFLAGS = $(PREPROCESSOR_FLAGS) $(GUI_PREPROCESSOR_FLAGS)
leabra_CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)
BUILT_SOURCES = leabra_TA_type.h leabra_TA_inst.h leabra_TA.cpp
