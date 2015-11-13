################################################################
#  Actually generate the config.h file

# IMPORTANT NOTE: config.h must be completely generic relative to the build type
# so that it can be installed in the global install dir, and won't conflict at all
# for different bulid types existing on the same machine

# note: putting in source but default is to put in CMAKE_BINARY_DIR
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake_config.h.in ${CMAKE_SOURCE_DIR}/taconfig.h)

# don't forget to install this guy!
install(FILES ${CMAKE_SOURCE_DIR}/taconfig.h DESTINATION ${EMERGENT_INCLUDE_DEST})

# VERSION is separate

# note: putting in source but default is to put in CMAKE_BINARY_DIR
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake_temt_version.h.in ${CMAKE_SOURCE_DIR}/temt_version.h)

# don't forget to install this guy!
install(FILES ${CMAKE_SOURCE_DIR}/temt_version.h DESTINATION ${EMERGENT_INCLUDE_DEST})

