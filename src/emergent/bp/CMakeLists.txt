########### emergent bp target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/bp)

CREATE_MAKETA_COMMAND(bp ${mypath} "${bp_HEADS_MAKETA}")


########### install files ###############

EMERGENT_INSTALL_HEADERS(${bp_HEADS})


