#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
PROJNAME = so
srcdir = @srcdir@
VPATH = @srcdir@

TAHEADS =\
	@abs_srcdir@/../ta/ta_TA_type.h \
	@abs_srcdir@/../pdp/pdp_TA_type.h \
	@abs_srcdir@/../so/so.h \
	@abs_srcdir@/../so/som.h \
	@abs_srcdir@/../so/cl.h \
	@abs_srcdir@/../so/zsh.h \
	@abs_srcdir@/../so/so_v3_compat.h

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

include $(top_srcdir)/Maketa.am
include $(top_srcdir)/mfstamp.am

noinst_LTLIBRARIES =\
	libso_TypeAccess@PDP_SUFFIX@.la \
	libso@PDP_SUFFIX@_@PACKAGE_VERSION@.la

libso@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(HEADS) $(SRCS)

libso@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @PDP_INCLUDES@
libso@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@

libso@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD = libso_TypeAccess@PDP_SUFFIX@.la

libso@PDP_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES = libso_TypeAccess@PDP_SUFFIX@.la

libso_TypeAccess@PDP_SUFFIX@_la_SOURCES =\
	so_TA.cpp \
	so_TA_type.h \
	so_TA_inst.h

libso_TypeAccess@PDP_SUFFIX@_la_CPPFLAGS = $(libso@PDP_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)

libso_TypeAccess@PDP_SUFFIX@_la_CXXFLAGS = -O0

so_def.h:
