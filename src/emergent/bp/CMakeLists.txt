########### emergent bp target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/bp)

CREATE_MAKETA_COMMAND(bp ${mypath} "${bp_HEADS_MAKETA}")


# setup dependency between dummy.cpp file included in dummy library and TA file we 
# actually want to generate now
SET_SOURCE_FILES_PROPERTIES(dummy.cpp
  PROPERTIES OBJECT_DEPENDS ${mypath}/bp_TA.cpp
)

# need a target to get anything to happen in this directory, so this is it
add_library(bp_dummy STATIC dummy.cpp)

########### install files ###############

install(FILES ${bp_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})


