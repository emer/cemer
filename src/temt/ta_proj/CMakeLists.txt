########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/ta_proj)

########### install files ###############

EMERGENT_INSTALL_HEADERS("${ta_proj_HEADS}")

