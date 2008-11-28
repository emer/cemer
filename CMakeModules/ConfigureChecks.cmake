####################################
#  Configure Checks

include(CheckIncludeFiles)
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )

CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)

# note: putting in source but default is to put in CMAKE_BINARY_DIR
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake_config.h.in ${CMAKE_SOURCE_DIR}/config.h)
