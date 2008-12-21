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
QT4_WRAP_CPP(temt_taiqtso_SRCS_MOC ${temt_taiqtso_HEADS_MOC})

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
	${temt_taiqtso_SRCS} ${temt_taiqtso_SRCS_MOC}
	${temt_ta_SRCS} ${temt_ta_SRCS_MOC}
	${temt_css_SRCS} ${temt_css_SRCS_MOC})

add_library(temt SHARED ${temtlib_SRCS})
  
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
