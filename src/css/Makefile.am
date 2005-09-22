PROJNAME = css
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

SRCS_GUI =\
	css_qttype.cpp \
	css_qttype.moc.cpp \
	css_qt.cpp \
	css_qttype.moc.cpp \
	css_qtdata.cpp \
	css_qtdata.moc.cpp \
	css_qtdialog.cpp

HEADS_GUI =\
	css_qttype.h \
	css_qt.h \
	css_qtdata.h \
	css_qtdialog.h
endif

SRCS =\
	css_parse.cpp \
	css_machine.cpp \
	css_basic_types.cpp \
	css_c_ptr_types.cpp \
	css_builtin.cpp \
	css_lex.cpp \
	ta_css.cpp \
	xmemory.cpp \
	css_special_math.cpp \
	css_misc_funs.cpp \
	$(SRCS_GUI) \
	css_ti.cpp

HEADS =\
	css_machine.h \
	css_basic_types.h \
	css_c_ptr_types.h \
	css_builtin.h \
	css_extern_support.h \
	ta_css.h \
	css_parse.h \
	css_special_math.h \
	hard_of_css.h \
	css_misc_funs.h \
	$(HEADS_GUI)

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libcss.la
libcss_la_SOURCES = $(HEADS) $(SRCS)
libcss_la_LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
libcss_la_CPPFLAGS =\
	$(PREPROCESSOR_FLAGS) \
	$(GUI_PREPROCESSOR_FLAGS) \
	-DYYDEBUG \
	-I$(abs_top_builddir)/src/taiqtso
libcss_la_CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)

bin_PROGRAMS = css
css_SOURCES = main.cpp

css_LDADD = libcss.la @abs_top_builddir@/src/ta/libta.la

if TA_GUI
css_LDADD += @abs_top_builddir@/src/taiqtso/libtaiqtso.la
endif

css_LDFLAGS = $(libcss_la_LDFLAGS)
css_CPPFLAGS = $(libcss_la_CPPFLAGS)
css_CFLAGS = $(libcss_la_CFLAGS)