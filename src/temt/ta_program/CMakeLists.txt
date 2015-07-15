########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/ta_program)

########### install files ###############

EMERGENT_INSTALL_HEADERS("${ta_program_HEADS}")

