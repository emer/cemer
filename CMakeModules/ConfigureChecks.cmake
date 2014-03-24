####################################
#  Configure Checks

# IMPORTANT NOTE: config.h must be completely generic relative to the build type
# so that it can be installed in the global install dir, and won't conflict at all
# for different bulid types existing on the same machine

#include(CheckIncludeFiles)
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
# CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)

# there is no real nogui build yet (or probably ever), so this is just manually set
set(TA_GUI TRUE)
# and always using Qt
set(TA_USE_QT TRUE)
# and always inventor too
set(TA_USE_INVENTOR TRUE)

# for now, obligatory V3 compatibility mode
#set(V3_COMPAT TRUE)

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

# this is passed to moc calls
set(MOC_DEFINES "")

# put debug on the command line, so that config.h is truly general
IF(CMAKE_BUILD_TYPE MATCHES "Debug") 
  add_definitions(-DDEBUG) 
  add_definitions(-DJSON_DEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug") 

##############################
# MPI = DMEM special stuff (set -DMPI_BUILD flag at compile time)
IF(MPI_BUILD)
  find_package(MPI REQUIRED)
  include_directories(${MPI_INCLUDE_PATH})
  set(EMERGENT_LIBRARIES ${EMERGENT_LIBRARIES} ${MPI_LIBRARY})
  add_definitions(-DDMEM_COMPILE)
  set(CMAKE_CXX_COMPILER mpicxx)
ENDIF(MPI_BUILD)

# Note: you have to include the ConfigureGenerate.cmake file to actually generate
# a config.h file
