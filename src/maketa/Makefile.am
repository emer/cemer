#*********************************************************
#  Process this file with automake to produce Makefile.in
#*********************************************************
LIBS=
DEFS=

PROJNAME = maketa

HEADS =\
	../ta/ta_stdef.h \
	../ta/ta_platform.h \
	../ta/ta_string.h \
	../ta/ta_variant.h \
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
	../ta/ta_variant.cpp \
	../ta/ta_list.cpp \
	../ta/ta_type.cpp \
	mta_parse.cpp \
	mta_lex.cpp \
	mta_constr.cpp \
	maketa.cpp

include $(top_srcdir)/mfstamp.am

mta_parse.cpp: mta_parse.y
	bison -dtvy mta_parse.y;
	sed -e "s/union/struct/g" y.tab.c >mta_parse.cpp;
	sed -e "s/union/struct/g" y.tab.h >mta_parse.hx;
	cmp -s mta_parse.hx mta_parse.h || (cp mta_parse.hx mta_parse.h);


ta_TA_type.h ta_TA_inst.h:
	if [ ! -f ta_TA_type.h ];\
	then touch -t 0412312359 ta_TA_type.h ta_TA_inst.h;fi

ta_ti.cpp ta_platform.cpp ta_platform_unix.cpp ta_string.cpp ta_variant.cpp ta_list.cpp ta_type.cpp:

bin_PROGRAMS = maketa

maketa_SOURCES = $(HEADS) $(SRCS)

maketa_CPPFLAGS =\
	-I@abs_srcdir@/../ta \
	-I@abs_srcdir@/.. \
	-I@abs_top_builddir@ \
	-I@abs_srcdir@ \
	-DNO_TA_BASE -g -O0

BUILT_SOURCES = ta_TA_type.h

CLEANFILES = ta_TA_type.h ta_TA_inst.h

