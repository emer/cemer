####################################
#  Configure Checks

#include(CheckIncludeFiles)
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
# CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)

# there is no real nogui build yet (or probably ever), so this is just manually set
set(TA_GUI TRUE)
# and always using Qt
set(TA_USE_QT TRUE)
# and always inventor too
set(TA_USE_INVENTOR TRUE)

# optional libraries -- found through the Findxx in main file
IF(GSL_FOUND)
  set(HAVE_LIBGSL TRUE)
ELSE(GSL_FOUND)
  set(HAVE_LIBGSL FALSE)
ENDIF (GSL_FOUND)

IF(ODE_FOUND)
  set(HAVE_LIBODE TRUE)
ELSE(ODE_FOUND)
  set(HAVE_LIBODE FALSE)
ENDIF (ODE_FOUND)

# note: putting in source but default is to put in CMAKE_BINARY_DIR
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake_config.h.in ${CMAKE_SOURCE_DIR}/config.h)
