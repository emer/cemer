########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/quarter)

########### install files ###############

EMERGENT_INSTALL_QUARTER_HEADERS("${quarter_HEADS}")

