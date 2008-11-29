########### emergent cs target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/cs)

CREATE_MAKETA_COMMAND(cs ${mypath} "${cs_HEADS_MAKETA}")

# setup dependency between dummy.cpp file included in dummy library and TA file we 
# actually want to generate now
SET_SOURCE_FILES_PROPERTIES(dummy.cpp
  PROPERTIES OBJECT_DEPENDS ${mypath}/cs_TA.cpp
)

# need a target to get anything to happen in this directory, so this is it
add_library(cs_dummy STATIC dummy.cpp)

########### install files ###############

install(FILES ${cs_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})

