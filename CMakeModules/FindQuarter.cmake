# find Quarter (3d Scenegraph library) includes and library
#
# QUARTER_INCLUDE_DIR - where the directory containing the Quarter headers can be found
# QUARTER_LIBRARY     - full path to the Quarter library
# QUARTER_FOUND       - TRUE if Quarter was found

FIND_PATH(QUARTER_INCLUDE_DIR Quarter.h
    /usr/include
    /usr/local/include
    /opt/local/include
    /usr/include/Quarter
    /usr/local/include/Quarter
    /opt/local/include/Quarter
    /Library/Frameworks/Quarter.framework/Headers
    $ENV{INCLUDE}
    ${COINDIR}/include/Quarter
)
#MESSAGE("QUARTER_INCLUDE_DIR=" ${QUARTER_INCLUDE_DIR})

# NOTE: General FIND_LIBRARY not working for some reason on Windows, but we want our own prereq
# anyways, and must use the debug version (because of runtime linkage) when in Debug anyways

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    IF (EXISTS ${COINDIR}/lib/quarter1d.lib)
      SET(QUARTER_LIBRARY "${COINDIR}/lib/quarter1d.lib")
    ENDIF (EXISTS ${COINDIR}/lib/quarter1.lib)
  else (CMAKE_BUILD_TYPE MATCHES "Debug") 
    IF (EXISTS ${COINDIR}/lib/quarter1.lib)
      SET(QUARTER_LIBRARY "${COINDIR}/lib/quarter1.lib")
    ENDIF (EXISTS ${COINDIR}/lib/quarter1.lib)
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(QUARTER_LIBRARY NAMES Quarter PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
  ) 
 endif (WIN32) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Quarter DEFAULT_MSG QUARTER_LIBRARY QUARTER_INCLUDE_DIR)
