#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = so
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	../../temt/ta/ta_TA_type.h \
	../network/emergent_TA_type.h \
	so.h \
	som.h \
	cl.h \
	zsh.h \
	so_v3_compat.h

HEADS=\
	so.h \
	som.h \
	cl.h \
	zsh.h \
	so_v3_compat.h

SRCS=\
	so.cpp \
	som.cpp \
	cl.cpp \
	zsh.cpp \
	so_v3_compat.cpp

include_HEADERS = $(HEADS) $(TAHEADS) so_TA_type.h so_TA_inst.h so_def.h

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libso_EmergentToolkit@EMERGENT_SUFFIX@.la \
	libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la

libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libso_EmergentToolkit@EMERGENT_SUFFIX@.la

libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libso_EmergentToolkit@EMERGENT_SUFFIX@.la

libso_EmergentToolkit@EMERGENT_SUFFIX@_la_SOURCES =\
	so_TA.cpp \
	so_TA_type.h \
	so_TA_inst.h

libso_EmergentToolkit@EMERGENT_SUFFIX@_la_CPPFLAGS = $(libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libso_EmergentToolkit@EMERGENT_SUFFIX@_la_CXXFLAGS = -O0

clean-local:
	-rm -f libso*
	if [ -n "`ls *\.moc\.cpp`" ];then rm `ls *\.moc\.cpp`;fi

so_def.h:
