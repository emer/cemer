########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(tapath ${PROJECT_SOURCE_DIR}/src/temt/ta)

CREATE_MAKETA_COMMAND(ta ${tapath} "${ta_HEADS_MAKETA}")

# setup dependency between dummy.cpp file included in dummy library and TA file we 
# actually want to generate now
SET_SOURCE_FILES_PROPERTIES(dummy.cpp
  PROPERTIES OBJECT_DEPENDS ${tapath}/ta_TA.cpp
)

# need a target to get anything to happen in this directory, so this is it
add_library(ta_dummy STATIC dummy.cpp)

########### install files ###############

#install(FILES  $(HEADS) $(TAHEADS) ta_TA_type.h ta_TA_inst.h DESTINATION /include)


##############################################################
##  extra stuff from original Makefile.am
#
#
#include_HEADERS = $(HEADS) $(TAHEADS) ta_TA_type.h ta_TA_inst.h
#
#include $(top_srcdir)/Maketa.am
#include $(top_srcdir)/Moc.am
#include $(top_srcdir)/mfstamp.am
#
#interceptor.moc.cpp:
#	$(QT_MOC) -o irenderarea.moc.cpp -I@abs_top_builddir@ -I@abs_top_builddir@/src/temt/ta interceptor.h
#
#noinst_LTLIBRARIES =\
#	libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
#	libtaEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
#
#libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_DEPENDENCIES =\
#	libtaEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
#
#libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(SRCS) $(HEADS)
#libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
#libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
#libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
#	libtaEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
#
#libtaEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =\
#	ta_TA.cpp
#	ta_TA_type.h
#	ta_TA_inst.h
#
#libtaEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS =\
#	$(libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS)
#
#libtaEmergentToolkit@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = -g -O0
#
#clean-local:
#	-rm -f libta*
#	if [ -n "`ls *\.moc\.cpp`" ];then rm `ls *\.moc\.cpp`;fi
#
#taglobal.h ta_maketa_defs.h qtdefs.h:
#
