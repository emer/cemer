########### temt lib target ###############

# this is where all the temt code gets built into a single library
# we read in the list of files from the relevant directories and then 
# add relative path for here, then just list them all as compile targets
# also, all moc generation happens here, but all maket generation must
# happen in the original source directory!!  see ta for example

# use relative pathing
set(mypath "..")

# taiqtso
include(${PROJECT_SOURCE_DIR}/src/temt/taiqtso/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_taiqtso_SRCS ${mypath}/taiqtso "${taiqtso_SRCS}")
ADD_PATH_TO_FILES(temt_taiqtso_HEADS_MOC ${mypath}/taiqtso "${taiqtso_HEADS_MOC}")
QT4_WRAP_CPP(temt_taiqtso_SRCS_MOC ${temt_taiqtso_HEADS_MOC} OPTIONS ${MOC_DEFINES})

# ta
include(${PROJECT_SOURCE_DIR}/src/temt/ta/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_SRCS ${mypath}/ta "${ta_SRCS}")
ADD_PATH_TO_FILES(temt_ta_HEADS_MOC ${mypath}/ta "${ta_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_ta_HEADS_MAKETA ${mypath}/ta "${ta_HEADS_MAKETA}")
QT4_WRAP_CPP(temt_ta_SRCS_MOC ${temt_ta_HEADS_MOC} OPTIONS ${MOC_DEFINES})
SET_TA_PROPS(ta ${PROJECT_SOURCE_DIR}/src/temt/ta)

# css
include(${PROJECT_SOURCE_DIR}/src/temt/css/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_css_SRCS ${mypath}/css "${css_SRCS}")
ADD_PATH_TO_FILES(temt_css_HEADS_MOC ${mypath}/css "${css_HEADS_MOC}")
QT4_WRAP_CPP(temt_css_SRCS_MOC ${temt_css_HEADS_MOC} OPTIONS ${MOC_DEFINES})

# just list whole set of files here
set(temtlib_SRCS
	${temt_taiqtso_SRCS} ${temt_taiqtso_SRCS_MOC}
	${temt_ta_SRCS} ${temt_ta_SRCS_MOC}
	${temt_css_SRCS} ${temt_css_SRCS_MOC})

add_library(temt SHARED ${temtlib_SRCS})

# 2nd arg is extra libs to link to
EMERGENT_LINK_LIBRARIES(temt "")

########### install files ###############

install(TARGETS temt
  LIBRARY DESTINATION lib
)
