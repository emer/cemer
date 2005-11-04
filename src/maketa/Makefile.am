PROJNAME = maketa
DEFS=
#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************

HEADS =\
	ta_stdef.h \
	ta_platform.h \
	ta_string.h \
	ta_list.h \
	ta_type.h \
	mta_parse.h \
	mta_constr.h \
	maketa.h

SRCS =\
	ta_ti.cpp \
	ta_platform.cpp \
	ta_platform_unix.cpp \
	ta_string.cpp \
	ta_list.cpp \
	ta_type.cpp \
	mta_parse.cpp \
	mta_lex.cpp \
	mta_constr.cpp \
	maketa.cpp

ta_TA_type.h ta_TA_inst.h:
	if [ ! -f ta_TA_type.h ];\
	then touch -t 196912311700.00 \
	@abs_top_srcdir@/src/maketa/ta_TA_type.h \
	@abs_top_srcdir@/src/maketa/ta_TA_inst.h;fi

bin_PROGRAMS = maketa
maketa_SOURCES = $(HEADS) $(SRCS)

BUILT_SOURCES = ta_TA_type.h ta_TA_inst.h
