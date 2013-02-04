########### emergent cs target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/cs)

CREATE_MAKETA_COMMAND(cs ${mypath} "${cs_HEADS_MAKETA}")


########### install files ###############

EMERGENT_INSTALL_HEADERS("${cs_HEADS}")

