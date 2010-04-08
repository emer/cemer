# find SoQt (3d Scenegraph library) includes and library
#
# SOQT_INCLUDE_DIR - where the directory containing the SoQt headers can be found
# SOQT_LIBRARY     - full path to the SoQt library
# SOQT_FOUND       - TRUE if SoQt was found

FIND_PATH(SOQT_INCLUDE_DIR SoQt.h
    /usr/include
    /usr/local/include
	/opt/local/include
    /usr/include/Inventor/Qt
    /usr/local/include/Inventor/Qt
	/opt/local/include/Inventor/Qt
	/Library/Frameworks/SoQt.framework/Headers
    $ENV{INCLUDE}
    ${COINDIR}/include/Inventor/Qt
)
#MESSAGE("SOQT_INCLUDE_DIR=" ${SOQT_INCLUDE_DIR})

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(SOQT_LIBRARY NAMES soqt1d PATHS
      "${COINDIR}/lib"
    )
  else (CMAKE_BUILD_TYPE MATCHES "Debug") 
    FIND_LIBRARY(SOQT_LIBRARY NAMES soqt1 PATHS
      "${COINDIR}/lib"
    )
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(SOQT_LIBRARY NAMES SoQt PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
  ) 
 endif (WIN32) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SoQt DEFAULT_MSG SOQT_LIBRARY SOQT_INCLUDE_DIR)
