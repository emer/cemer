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

# NOTE: General FIND_LIBRARY not working for some reason on Windows, but we want our own prereq
# anyways, and must use the debug version (because of runtime linkage) when in Debug anyways

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    IF (EXISTS ${COINDIR}/lib/soqt1d.lib)
      SET(SOQT_LIBRARY "${COINDIR}/lib/soqt1d.lib")
    ENDIF (EXISTS ${COINDIR}/lib/soqt1d.lib)
  else (CMAKE_BUILD_TYPE MATCHES "Debug") 
    IF (EXISTS ${COINDIR}/lib/soqt1.lib)
      SET(SOQT_LIBRARY "${COINDIR}/lib/soqt1.lib")
    ENDIF (EXISTS ${COINDIR}/lib/soqt1.lib)
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(SOQT_LIBRARY NAMES SoQt PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
  ) 
 endif (WIN32) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SoQt DEFAULT_MSG SOQT_LIBRARY SOQT_INCLUDE_DIR)
