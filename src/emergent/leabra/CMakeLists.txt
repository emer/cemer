########### emergent leabra target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/leabra)

CREATE_MAKETA_COMMAND(leabra ${mypath} "${leabra_HEADS_MAKETA}")

########### install files ###############

EMERGENT_INSTALL_HEADERS("${leabra_HEADS}")
