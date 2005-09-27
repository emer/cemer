PROJNAME = taiqtso
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

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
	-DTA_GUI \
	-DTA_USE_QT \
	-DTA_USE_INVENTOR \
	-DQT_SHARED

libtaiqtso@PDP_SUFFIX@_la_LDFLAGS = @X_LIBS@

#**************************************************
#
#                      GUI
#
#**************************************************
if TA_GUI

libtaiqtso@PDP_SUFFIX@_la_LIBADD =\
	@X_EXTRA_LIBS@ \
	@X_PRE_LIBS@ \
	@QT_LDADD@

endif
#**************************************************
#
#                      DEBUG
#
#**************************************************
if DEBUG

libtaiqtso@PDP_SUFFIX@_la_CPPFLAGS +=\
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

libtaiqtso@PDP_SUFFIX@_la_CPPFLAGS += -DDMEM_COMPILE
libtaiqtso@PDP_SUFFIX@_la_LIBADD = @MPILIBS@

endif
#**************************************************
#
#                      LINUX
#
#**************************************************
if LINUX

libtaiqtso@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX

endif
#**************************************************
#
#                      DARWIN
#
#**************************************************
if DARWIN

libtaiqtso@PDP_SUFFIX@_la_CPPFLAGS += -DLINUX -DDARWIN

endif
#**************************************************
#
#                      DEBUG
#
#**************************************************
clean-local:
	-rm *TA*
	-rm *moc*
	-RM *~