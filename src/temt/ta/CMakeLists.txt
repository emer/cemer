########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/ta)

CREATE_MAKETA_COMMAND(ta ${mypath} "${ta_HEADS_MAKETA}")

# setup dependency between dummy.cpp file included in dummy library and TA file we 
# actually want to generate now
SET_SOURCE_FILES_PROPERTIES(dummy.cpp
  PROPERTIES OBJECT_DEPENDS ${mypath}/ta_TA.cpp
)

# need a target to get anything to happen in this directory, so this is it
add_library(ta_dummy STATIC dummy.cpp)

########### install files ###############

install(FILES ${ta_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})

