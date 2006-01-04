PROJNAME = maketa
DEFS=

do-mfstamp-am do-mfstamp: Makefile.in
	@echo "timestamp for all Makefile.in files" > $(top_builddir)/mfstamp
	@touch $@

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
	then touch -t 0412312359 ta_TA_type.h ta_TA_inst.h;fi

bin_PROGRAMS = maketa
maketa_SOURCES = $(HEADS) $(SRCS)
maketa_CPPFLAGS = -I@abs_srcdir@
BUILT_SOURCES = ta_TA_type.h
CLEANFILES = ta_TA_type.h ta_TA_inst.h
EXTRA_DIST =\
	ta_base.h \
	ta_constr.h \
	ta_def.h \
	ta_dump.h \
	ta_filer.h \
	ta_group.h \
	ta_matrix.h \
	ta_parse.h \
	ta_script.h \
	taglobal.h

