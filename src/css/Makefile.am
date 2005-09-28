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

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

lib_LTLIBRARIES = libcss@PDP_SUFFIX@.la
libcss@PDP_SUFFIX@_la_SOURCES = $(HEADS) $(SRCS)
libcss@PDP_SUFFIX@_la_CPPFLAGS =\
	-I$(srcdir)/../ta

bin_PROGRAMS = css@PDP_SUFFIX@++
css@PDP_SUFFIX@___SOURCES = main.cpp
css@PDP_SUFFIX@___LDADD =\
	libcss@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@.la

libcss@PDP_SUFFIX@_la_LIBADD = @MPILIBS@

css@PDP_SUFFIX@___LDFLAGS = $(libcss@PDP_SUFFIX@_la_LDFLAGS)
css@PDP_SUFFIX@___CPPFLAGS = $(libcss@PDP_SUFFIX@_la_CPPFLAGS)
css@PDP_SUFFIX@___CXXFLAGS = $(libcss@PDP_SUFFIX@_la_CXXFLAGS)

AM_CXXFLAGS = -fno-implicit-templates

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI

css@PDP_SUFFIX@___LDADD += @abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la

libcss@PDP_SUFFIX@_la_LIBADD += @X_EXTRA_LIBS@ @X_PRE_LIBS@ @QT_LDADD@

libcss@PDP_SUFFIX@_la_LDFLAGS = @X_LIBS@

libcss@PDP_SUFFIX@_la_CPPFLAGS +=\
	-I../src/taiqtso \
	-I@srcdir@/../taiqtso \
	@X_CFLAGS@ \
	-I/usr/include/freetype2 \
	@QT_CXXFLAGS@ \
	-DTA_GUI \
	-DTA_USE_QT \
	-DTA_USE_INVENTOR \
	-DQT_SHARED

endif
#**************************************************
#
#                      NOGUI
#
#**************************************************
if NO_TA_GUI

libcss@PDP_SUFFIX@_la_CPPFLAGS += -DTA_NO_GUI

endif
#**************************************************
#
#                      DEBUG
#
#**************************************************
if DEBUG

libcss@PDP_SUFFIX@_la_CPPFLAGS +=\
	-DYYDEBUG \
	-DDEBUG \
	-DTA_PROFILE \
	-DHAVE_POSIX_SIGNALS=1 \
	-DRETSIGTYPE=void

endif
#**************************************************
#
#                      MPI
#
#**************************************************
if MPI

libcss@PDP_SUFFIX@_la_CPPFLAGS += -DDMEM_COMPILE

endif
#**************************************************
#
#                      LINUX
#
#**************************************************
if LINUX

libcss@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX

endif
#**************************************************
#
#                      DARWIN
#
#**************************************************
if DARWIN

libcss@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX -DDARWIN

endif

clean-local:
	-rm *TA*
	-rm *moc*
	-rm *~
