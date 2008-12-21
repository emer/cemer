# find TEMT (The Emergent Toolkit) includes and library
#
# TEMT_INCLUDE_DIR - where the directory containing the TEMT headers can be found
# TEMT_LIBRARY     - full path to the TEMT library
# TEMT_FOUND       - TRUE if TEMT was found

# IMPORTANT: the EMERGENT_SUFFIX must be set by SetBuildSuffix.cmake module prior to this!

if (WIN32)
  if (NOT EXISTS "${EMERGENTDIR}/include/ta_type.h")
    message(FATAL_ERROR "EMERGENTDIR environment variable must point to Emergent install directory!")
  endif (NOT EXISTS "${EMERGENTDIR}/include/ta_type.h")
  set(TEMT_INCLUDE_DIR "${EMERGENTDIR}/include")
else (WIN32)
  FIND_PATH(TEMT_INCLUDE_DIR ta_type.h
    $ENV{EMERGENT_INCLUDE_DIR}
    /usr/include/Emergent
    /usr/local/include/Emergent
    /opt/local/include/Emergent
  )
endif (WIN32)

if (WIN32)
  #note: there are issues on windows trying to find libs, so:
  find_file(TEMT_LIBRARY "temt${EMERGENT_SUFFIX}.lib" PATHS
    "${EMERGENTDIR}/lib"
    "${EMERGENTDIR}/lib/${CMAKE_BUILD_TYPE}" #don't use CMAKE_CFG_INTDIR not valid on "NMake Makefiles"
    NO_DEFAULT_PATH
  )
  if (NOT EXISTS ${TEMT_LIBRARY})
    message(FATAL_ERROR "EMERGENTDIR environment variable must point to Emergent install directory!")
  endif (NOT EXISTS ${TEMT_LIBRARY})
  # Windows import/export control -- since we are trying to find it, we assume importing
  add_definitions(-DTA_DLL)
else (WIN32)
  FIND_LIBRARY(TEMT_LIBRARY NAMES "temt${EMERGENT_SUFFIX}" PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
  )
endif (WIN32)

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Temt DEFAULT_MSG TEMT_LIBRARY TEMT_INCLUDE_DIR)
