########### temt lib target ###############

# this is where all the temt code gets built into a single library
# we read in the list of files from the relevant directories and then 
# add relative path for here, then just list them all as compile targets
# also, all moc generation happens here, but all maket generation must
# happen in the original source directory!!  see ta for example

# cmake 2.4 needs absolute pathing to find _TA.cpp file
#set(mypath "..")
set(mypath ${PROJECT_SOURCE_DIR}/src/temt)

# taiqtso
include(${PROJECT_SOURCE_DIR}/src/temt/taiqtso/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_taiqtso_SRCS ${mypath}/taiqtso "${taiqtso_SRCS}")
ADD_PATH_TO_FILES(temt_taiqtso_HEADS_MOC ${mypath}/taiqtso "${taiqtso_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_taiqtso_SRCS_UI ${mypath}/taiqtso "${taiqtso_SRCS_UI}")
QT4_WRAP_CPP(temt_taiqtso_SRCS_MOC ${temt_taiqtso_HEADS_MOC})
QT4_WRAP_UI(temt_taiqtso_SRCS_UIC ${temt_taiqtso_SRCS_UI})

# ta
include(${PROJECT_SOURCE_DIR}/src/temt/ta/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_SRCS ${mypath}/ta "${ta_SRCS}")
ADD_PATH_TO_FILES(temt_ta_HEADS_MOC ${mypath}/ta "${ta_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_ta_HEADS_MAKETA ${mypath}/ta "${ta_HEADS_MAKETA}")
QT4_WRAP_CPP(temt_ta_SRCS_MOC ${temt_ta_HEADS_MOC})
SET_TA_PROPS(ta ${PROJECT_SOURCE_DIR}/src/temt/ta)

# css
include(${PROJECT_SOURCE_DIR}/src/temt/css/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_css_SRCS ${mypath}/css "${css_SRCS}")
ADD_PATH_TO_FILES(temt_css_HEADS_MOC ${mypath}/css "${css_HEADS_MOC}")
QT4_WRAP_CPP(temt_css_SRCS_MOC ${temt_css_HEADS_MOC})

# just list whole set of files here
set(temtlib_SRCS
	${temt_taiqtso_SRCS} ${temt_taiqtso_SRCS_MOC} ${temt_taiqtso_SRCS_UIC}
	${temt_ta_SRCS} ${temt_ta_SRCS_MOC}
	${temt_css_SRCS} ${temt_css_SRCS_MOC})

add_library(temt SHARED ${temtlib_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/svnrev.h)

########### get svn revision ###############
# http://stackoverflow.com/questions/3780667/use-cmake-to-get-build-time-svn-revision
#
# include the output directory, where the svnrev.h file is generated
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# providing a filename that will never exist ensures this target
# is built every time
add_custom_target(svn_revision_header ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doesnotexist.h)

# this purposefully does not create the doesnotexist.h file!
# creates svnrev.h using cmake script
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doesnotexist.h
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/GetSvn.cmake)

# svnrev.h is a generated file
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/svnrev.h
    PROPERTIES GENERATED TRUE
    HEADER_FILE_ONLY TRUE)

# make the library depend on the svn_revision_header
add_dependencies(temt svn_revision_header)
#
########### end get svn revision ###############


# 2nd arg is extra libs to link to
EMERGENT_LINK_LIBRARIES(temt "")

# Windows import/export control
if (WIN32)
  add_definitions(-DTA_DLL -DTA_EXPORTS)
endif (WIN32)

########### install files ###############

if (WIN32)
  install(TARGETS temt
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
  )
else (WIN32)
  install(TARGETS temt
    LIBRARY DESTINATION lib
  )
endif (WIN32)
