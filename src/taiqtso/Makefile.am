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

lib_LTLIBRARIES = libtaiqtso@PDP_SUFFIX@.la
libtaiqtso@PDP_SUFFIX@_la_SOURCES = $(SRCS) $(HEADS)
libtaiqtso@PDP_SUFFIX@_la_CPPFLAGS =\
        -I@srcdir@/../ta \
        @X_CFLAGS@ \
        -I/usr/include/freetype2 \
	 @QT_CXXFLAGS@ \
	-I${QTDIR}/include

clean-local:
	-rm *moc*
