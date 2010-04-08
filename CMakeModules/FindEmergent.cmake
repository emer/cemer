# find EMERGENT (Emergent Neural Network Simulation System) includes and library
#
# EMERGENT_INCLUDE_DIR - where the directory containing the EMERGENT headers can be found
# EMERGENT_LIBRARY     - full path to the EMERGENT library
# EMERGENT_LIBRARIES   - both the temt and emergent libraries
# EMERGENT_FOUND       - TRUE if EMERGENT was found

# IMPORTANT: the EMERGENT_SUFFIX must be set by SetBuildSuffix.cmake module prior to this!

# first find temt -- emergent requires that, and sets TEMT_INCLUDE_DIR and TEMT_LIBRARY
find_package(Temt REQUIRED)

if (WIN32)
  #shortcut: we already found everything in Find(temt)
  set(EMERGENT_INCLUDE_DIR ${TEMT_INCLUDE_DIR})
else (WIN32)
  FIND_PATH(EMERGENT_INCLUDE_DIR ta_type.h
    $ENV{EMERGENT_INCLUDE_DIR}
    /usr/local/include/Emergent
    /usr/include/Emergent
    /opt/local/include/Emergent
    /opt/include/Emergent
  )
endif (WIN32)

if (WIN32)
  find_library(EMERGENT_LIBRARY "emergentlib${EMERGENT_SUFFIX}" PATHS
    "${EMERGENTDIR}/lib"
    "${EMERGENTDIR}/lib/${CMAKE_BUILD_TYPE}" #don't use CMAKE_CFG_INTDIR not valid on "NMake Makefiles"
  )
  if (NOT EXISTS "${EMERGENT_LIBRARY}")
    message(FATAL_ERROR "EMERGENTDIR environment variable must point to Emergent install directory!")
  endif (NOT EXISTS "${EMERGENT_LIBRARY}")
  # Windows import/export control -- since we are trying to find it, we assume importing
  add_definitions(-DEMERGENT_DLL)
else (WIN32)
  FIND_LIBRARY(EMERGENT_LIBRARY NAMES "emergentlib${EMERGENT_SUFFIX}" PATHS
    /usr/local/lib
    /usr/lib
    /opt/local/lib
    /opt/lib
  )
endif (WIN32)

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Emergent DEFAULT_MSG EMERGENT_LIBRARY EMERGENT_INCLUDE_DIR)

set(EMERGENT_LIBRARIES ${TEMT_LIBRARY} ${EMERGENT_LIBRARY})
