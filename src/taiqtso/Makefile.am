#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

PROJNAME = taiqtso
srcdir = @srcdir@
VPATH = @srcdir@

HEADS =\
	taiqtso_def.h \
	iflowlayout.h \
	ilabel.h \
	icliptoolwidget.h \
	ispinbox.h \
	iwidgetlist.h \
	itreewidget.h \
	ilineedit.h \
	icheckbox.h \
	icombobox.h \
	igeometry.h \
	ieditgrid.h \
	safeptr_so.h \
	imisc_so.h \
	irenderarea.h \
	idimedit.h \
	interceptor.h \
	qconsole.h \
	test_iflowbox.h \
	test_iflowlayout.h \
	test_simplewidgets.h \
	test_itreeview.h \
	test_mainwindow.h

SRCS =\
	iflowlayout.cpp \
	ilabel.cpp \
	ilabel.moc.cpp \
	icliptoolwidget.cpp \
	icliptoolwidget.moc.cpp \
	ispinbox.cpp \
	ispinbox.moc.cpp \
	iwidgetlist.cpp \
	itreewidget.cpp \
	itreewidget.moc.cpp \
	ilineedit.cpp \
	ilineedit.moc.cpp \
	icheckbox.cpp \
	icheckbox.moc.cpp \
	icombobox.cpp \
	icombobox.moc.cpp \
	igeometry.cpp \
	ieditgrid.cpp \
	ieditgrid.moc.cpp \
	idimedit.cpp \
	idimedit.moc.cpp \
	interceptor.cpp \
	interceptor.moc.cpp \
	qconsole.cpp \
	qconsole.moc.cpp \
	safeptr_so.cpp \
	imisc_so.cpp \
	irenderarea.cpp \
	irenderarea.moc.cpp \
	taiqtso_ti.cpp

include $(top_srcdir)/Moc.am
include $(top_srcdir)/mfstamp.am

irenderarea.moc.cpp:
	$(QT_MOC) -DTA_USE_INVENTOR -o irenderarea.moc.cpp irenderarea.h

noinst_LTLIBRARIES = libtaiqtso@PDP_SUFFIX@@PDP_VERSION@.la

libtaiqtso@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = $(SRCS) $(HEADS)

libtaiqtso@PDP_SUFFIX@@PDP_VERSION@_la_CPPFLAGS =\
	-I@srcdir@/.. \
	-I@srcdir@ \
	-I@abs_top_builddir@ \
	-I@srcdir@/../ta

libtaiqtso@PDP_SUFFIX@@PDP_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

