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

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(QUARTER_LIBRARY NAMES quarter1d PATHS
      ${COINDIR}/lib
    ) 
  else (CMAKE_BUILD_TYPE MATCHES "Debug") 
    FIND_LIBRARY(QUARTER_LIBRARY NAMES quarter1 PATHS
      ${COINDIR}/lib
    ) 
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(QUARTER_LIBRARY NAMES Quarter PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
  ) 
endif (WIN32) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Quarter DEFAULT_MSG QUARTER_LIBRARY QUARTER_INCLUDE_DIR)
