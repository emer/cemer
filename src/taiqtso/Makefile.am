#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

PROJNAME = taiqtso
DEFS=
srcdir = @srcdir@
VPATH = @srcdir@

HEADS =\
	taiqtso_def.h \
	iflowlayout.h \
	ispinbox.h \
	iwidgetlist.h \
	iflowbox.h \
	itreeview.h \
	ilineedit.h \
	icheckbox.h \
	icombobox.h \
	igeometry.h \
	itransformer.h \
	ieditgrid.h \
	safeptr_so.h \
	imisc_so.h \
	irenderarea.h \
	test_iflowbox.h \
	test_iflowlayout.h \
	test_simplewidgets.h \
	test_itreeview.h \
	test_mainwindow.h

SRCS =\
	iflowlayout.cpp \
	ispinbox.cpp \
	ispinbox.moc.cpp \
	iwidgetlist.cpp \
	iflowbox.cpp \
	iflowbox.moc.cpp \
	itreeview.cpp \
	itreeview.moc.cpp \
	ilineedit.cpp \
	ilineedit.moc.cpp \
	icheckbox.cpp \
	icheckbox.moc.cpp \
	icombobox.cpp \
	icombobox.moc.cpp \
	igeometry.cpp \
	itransformer.cpp \
	ieditgrid.cpp \
	ieditgrid.moc.cpp \
	safeptr_so.cpp \
	imisc_so.cpp \
	irenderarea.cpp \
	irenderarea.moc.cpp \
	taiqtso_ti.cpp

include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libtaiqtso@PDP_SUFFIX@.la
libtaiqtso@PDP_SUFFIX@_la_SOURCES = $(SRCS) $(HEADS)
libtaiqtso@PDP_SUFFIX@_la_CPPFLAGS =\
	-I@srcdir@ \
	-I@abs_top_builddir@ \
	-I@srcdir@/../ta

libtaiqtso@PDP_SUFFIX@_la_CXXFLAGS = -g -O2

clean-local:
	-rm *moc*
