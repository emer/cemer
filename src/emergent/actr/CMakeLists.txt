########### emergent actr target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/actr)

CREATE_MAKETA_COMMAND(actr ${mypath} "${actr_HEADS_MAKETA}")


########### install files ###############

EMERGENT_INSTALL_HEADERS("${actr_HEADS}")

