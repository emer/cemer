PROJNAME = maketa
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

HEADS =\
	../ta/ta_stdef.h \
	../ta/ta_platform.h \
	../ta/ta_string.h \
	../ta/ta_list.h \
	../ta/ta_type.h \
	mta_parse.h \
	mta_constr.h \
	maketa.h

SRCS =\
	../ta/ta_ti.cpp \
	../ta/ta_platform.cpp \
	../ta/ta_platform_unix.cpp \
	../ta/ta_string.cpp \
	../ta/ta_list.cpp \
	../ta/ta_type.cpp \
	mta_parse.cpp \
	mta_lex.cpp \
	mta_constr.cpp \
	maketa.cpp

ta_TA_type.h ta_TA_inst.h:
	if [ ! -f ta_TA_type.h ];\
	then touch -t 196912311700.00 \
	@abs_top_srcdir@/src/ta/ta_TA_type.h \
	@abs_top_srcdir@/src/ta/ta_TA_inst.h;fi

bin_PROGRAMS = maketa
maketa_SOURCES = $(HEADS) $(SRCS)
maketa_CPPFLAGS =\
	-I@srcdir@/../ta \
	-DMAKING_MAKETA

BUILT_SOURCES = ta_TA_type.h ta_TA_inst.h

#**************************************************
#
#                     LINUX
#
#**************************************************
if LINUX

maketa_CPPFLAGS += -DLINUX

endif
#**************************************************
#
#                     DARWIN
#
#**************************************************
if DARWIN

maketa_CPPFLAGS += -DLINUX -DDARWIN

endif
#**************************************************
#
#                     DEBUG
#
#**************************************************
if DEBUG

maketa_CPPFLAGS +=\
	-DYYDEBUG \
	-DDEBUG \
	-DTA_PROFILE \
	-DHAVE_POSIX_SIGNALS=1

endif
