#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = css
srcdir = @srcdir@
VPATH = @srcdir@

if TA_GUI

SRCS_GUI =\
	css_qtconsole.cpp \
	css_qtconsole.moc.cpp \
	css_qttype.cpp \
	css_qttype.moc.cpp \
	css_qt.cpp \
	css_qtdata.cpp \
	css_qtdata.moc.cpp \
	css_qtdialog.cpp

HEADS_GUI =\
	css_qtconsole.h \
	css_qttype.h \
	css_qt.h \
	css_qtdata.h \
	css_qtdialog.h
endif

SRCS =\
	css_console.cpp \
	css_console.moc.cpp \
	css_parse.cpp \
	css_machine.cpp \
	css_machine.moc.cpp \
	css_basic_types.cpp \
	css_c_ptr_types.cpp \
	css_builtin.cpp \
	css_lex.cpp \
	css_ta.cpp \
	xmemory.cpp \
	css_misc_funs.cpp \
	readline_win.cpp \
	$(SRCS_GUI) \
	css_ti.cpp

HEADS =\
	css_console.h \
	css_machine.h \
	css_basic_types.h \
	css_c_ptr_types.h \
	css_builtin.h \
	css_extern_support.h \
	css_ta.h \
	css_parse.h \
	hard_of_css.h \
	css_misc_funs.h \
	$(HEADS_GUI)

include_HEADERS = $(HEADS) $(TAHEADS)

include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

css_qtconsole.moc.cpp:
	$(QT_MOC) -o css_qtconsole.moc.cpp -I@abs_top_builddir@ -I@abs_top_builddir@/src/css -I@abs_top_builddir@/src/temt/ta css_qtconsole.h

make_css_parse:
	bison -dtvy css_parse.y;
	sed -e "s/union/struct/g" y.tab.c >css_parse.cpp;
	sed -e "s/union/struct/g" y.tab.h >css_parse.hxx;
	cmp css_parse.hxx css_parse.h || cp css_parse.hxx css_parse.h;

noinst_LTLIBRARIES = libcss@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
libcss@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)
libcss@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
libcss@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

clean-local:
	-rm -f libcss*

cssdef.h:
