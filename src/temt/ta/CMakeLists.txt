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
#
#interceptor.moc.cpp:
#	$(QT_MOC) -o irenderarea.moc.cpp -I@abs_top_builddir@ -I@abs_top_builddir@/src/temt/ta interceptor.h
#
