########### temt lib target ###############

# this is where all the temt code gets built into a single library
# we read in the list of files from the relevant directories and then 
# add relative path for here, then just list them all as compile targets
# also, all moc generation happens here, but all maket generation must
# happen in the original source directory!!  see ta for example

# use relative pathing
set(temtpath "..")

# taiqtso
include(${PROJECT_SOURCE_DIR}/src/temt/taiqtso/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_taiqtso_SRCS ${temtpath}/taiqtso "${taiqtso_SRCS}")
ADD_PATH_TO_FILES(temt_taiqtso_HEADS_MOC ${temtpath}/taiqtso "${taiqtso_HEADS_MOC}")

# ta
include(${PROJECT_SOURCE_DIR}/src/temt/ta/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_SRCS ${temtpath}/ta "${ta_SRCS}")
ADD_PATH_TO_FILES(temt_ta_HEADS_MOC ${temtpath}/ta "${ta_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_ta_HEADS_MAKETA ${temtpath}/ta "${ta_HEADS_MAKETA}")

# css
include(${PROJECT_SOURCE_DIR}/src/temt/css/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_css_SRCS ${temtpath}/css "${css_SRCS}")
ADD_PATH_TO_FILES(temt_css_HEADS_MOC ${temtpath}/css "${css_HEADS_MOC}")

# moc file generation
QT4_WRAP_CPP(temt_taiqtso_SRCS_MOC ${temt_taiqtso_HEADS_MOC})
QT4_WRAP_CPP(temt_ta_SRCS_MOC ${temt_ta_HEADS_MOC})
QT4_WRAP_CPP(temt_css_SRCS_MOC ${temt_css_HEADS_MOC})

# set special compile properties for ta_TA.cpp file
SET_TA_PROPS(ta ${PROJECT_SOURCE_DIR}/src/temt/ta)

# just list whole set of files here
set(temtlib_SRCS
	${temt_taiqtso_SRCS} ${temt_taiqtso_SRCS_MOC}
	${temt_ta_SRCS} ${temt_ta_SRCS_MOC}
	${temt_css_SRCS} ${temt_css_SRCS_MOC})

add_library(temt SHARED ${temtlib_SRCS})

target_link_libraries(temt ${EMERGENT_LIBRARIES})

set_target_properties(temt PROPERTIES
 VERSION ${EMERGENT_LIB_VERSION}
 SOVERSION ${EMERGENT_LIB_SOVERSION}
)


########### install files ###############

#install(TARGETS temt@EMERGENT_SUFFIX@-@PACKAGE_VERSION@ ${INSTALL_TARGETS_DEFAULT_ARGS})


