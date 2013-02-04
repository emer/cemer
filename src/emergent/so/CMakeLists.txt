########### emergent so target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/so)

CREATE_MAKETA_COMMAND(so ${mypath} "${so_HEADS_MAKETA}")


########### install files ###############

EMERGENT_INSTALL_HEADERS("${so_HEADS}")

