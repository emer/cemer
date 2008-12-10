########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/ta)

CREATE_MAKETA_COMMAND(ta ${mypath} "${ta_HEADS_MAKETA}")


########### install files ###############

install(FILES ${ta_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})

