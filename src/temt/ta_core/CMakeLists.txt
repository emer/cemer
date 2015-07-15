########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/ta_core)

########### install files ###############

EMERGENT_INSTALL_HEADERS("${ta_core_HEADS}")

