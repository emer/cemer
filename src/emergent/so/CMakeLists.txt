########### emergent so target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/so)

CREATE_MAKETA_COMMAND(so ${mypath} "${so_HEADS_MAKETA}")


########### install files ###############

install(FILES ${so_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})

