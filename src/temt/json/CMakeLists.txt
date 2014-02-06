########### css target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/json)

########### install files ###############

EMERGENT_INSTALL_HEADERS("${json_HEADS}")

