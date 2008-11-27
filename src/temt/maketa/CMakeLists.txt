########### maketa target ###############

set(maketa_HEADS
../ta/ta_stdef.h
../ta/ta_platform.h
../ta/ta_string.h
../ta/ta_variant.h
../ta/ta_mtrnd.h
../ta/ta_list.h
../ta/ta_type.h
mta_parse.h
mta_constr.h
maketa.h
)

set(maketa_SRCS
../ta/ta_ti.cpp
../ta/ta_platform.cpp
../ta/ta_string.cpp
../ta/ta_variant.cpp
../ta/ta_mtrnd.cpp
../ta/ta_list.cpp
../ta/ta_type.cpp
mta_parse.cpp
mta_lex.cpp
mta_constr.cpp
mta_gendoc.cpp
maketa.cpp
)

set(maketa_FILES ${makta_HEADS} ${maketa_SRCS})

add_executable(maketa ${maketa_FILES})

target_link_libraries(maketa ${QT_LIBRARIES})

include_directories(${QT_INCLUDES} ../ ../ta)

# note: special definitions
add_definitions(-DNO_TA_BASE -O2 -g)

#install(TARGETS maketa ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

#install(FILES  $(HEADS) $(TAHEADS) DESTINATION /include)

##############################################################
##  extra stuff from original Makefile.am

#make_mta_parse:
#	bison -dtvy mta_parse.y;
#	sed -e "s/union/struct/g" y.tab.c >mta_parse.cpp;
#	sed -e "s/union/struct/g" y.tab.h >mta_parse.hxx;
#	cmp mta_parse.hxx mta_parse.h || cp mta_parse.hxx mta_parse.h;
#
#ta_TA_type.h ta_TA_inst.h:
#	if [ ! -f ta_TA_type.h ];\
#	then touch -t 0412312359 ta_TA_type.h ta_TA_inst.h;fi
#
#bin_PROGRAMS = maketa
#maketa_SOURCES = $(HEADS) $(SRCS)
#maketa_CPPFLAGS = @EMERGENT_INCLUDES@ -DNO_TA_BASE @MAKETA_OPT@
#BUILT_SOURCES = ta_TA_type.h
#CLEANFILES = ta_TA_type.h ta_TA_inst.h
