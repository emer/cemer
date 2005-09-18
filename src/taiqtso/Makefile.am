PROJNAME = taiqtso
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

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
	moc_ispinbox.cpp \
	iwidgetlist.cpp \
	iflowbox.cpp \
	moc_iflowbox.cpp \
	itreeview.cpp \
	moc_itreeview.cpp \
	ilineedit.cpp \
	moc_ilineedit.cpp \
	icheckbox.cpp \
	moc_icheckbox.cpp \
	icombobox.cpp \
	moc_icombobox.cpp \
	igeometry.cpp \
	itransformer.cpp \
	ieditgrid.cpp \
	moc_ieditgrid.cpp \
	safeptr_so.cpp \
	imisc_so.cpp \
	irenderarea.cpp \
	moc_irenderarea.cpp \
	taiqtso_ti.cpp

include $(top_srcdir)/Conditionals.am
include $(top_srcdir)/Moc.am

lib_LTLIBRARIES = libtaiqtso.la
libtaiqtso_la_SOURCES = $(HEADS) $(SRCS)
libtaiqtso_la_LDFLAGS = $(LINKER_FLAGS) $(GUI_LINKER_FLAGS)
libtaiqtso_la_CPPFLAGS = $(PREPROCESSOR_FLAGS) $(GUI_PREPROCESSOR_FLAGS)
libtaiqtso_la_CFLAGS = $(COMPILER_FLAGS) $(GUI_COMPILER_FLAGS)
