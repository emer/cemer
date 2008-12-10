########### emergent network target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/network)

CREATE_MAKETA_COMMAND(network ${mypath} "${network_HEADS_MAKETA}")


########### install files ###############

install(FILES ${network_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})
