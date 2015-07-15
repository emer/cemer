########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/virt_env)

########### install files ###############

EMERGENT_INSTALL_HEADERS("${virt_env_HEADS}")

