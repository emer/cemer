# find Coin (3d Scenegraph library) includes and library
#
# COIN_INCLUDE_DIR - where the directory containing the Coin headers can be found
# COIN_LIBRARY     - full path to the Coin library
# COIN_FOUND       - TRUE if Coin was found

FIND_PATH(COIN_INCLUDE_DIR SbVec3f.h
    /usr/include
    /usr/local/include
	/opt/local/include
    /usr/include/Inventor
    /usr/local/include/Inventor
	/opt/local/include/Inventor
	/Library/Frameworks/Inventor.framework/Headers
    $ENV{INCLUDE}
    ${COINDIR}/include/Inventor
)

# NOTE: Mac is not finding the coin framework because the name is Inventor but the lib is Coin!
# SoQt works perfectly just right out of the bag
# NOTE: General FIND_LIBRARY not working for some reason on Windows, but we want our own prereq
# anyways, and must use the debug version (because of runtime linkage) when in Debug anyways

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    IF (EXISTS ${COINDIR}/lib/coin2d.lib)
      SET(COIN_LIBRARY "${COINDIR}/lib/coin2d.lib")
    ENDIF (EXISTS ${COINDIR}/lib/coin2d.lib)
  else (CMAKE_BUILD_TYPE MATCHES "Debug") 
    IF (EXISTS ${COINDIR}/lib/coin2.lib)
      SET(COIN_LIBRARY "${COINDIR}/lib/coin2.lib")
    ENDIF (EXISTS ${COINDIR}/lib/coin2.lib)
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(COIN_LIBRARY NAMES Coin PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /Library/Frameworks/Inventor.framework/Libraries
    ${COINDIR}/lib
  )
 endif (WIN32) 

#MESSAGE("COIN_LIBRARY=" ${COIN_LIBRARY})
# special work-around for Coin Framework
IF (APPLE)
  IF (NOT COIN_LIBRARY)
    IF (EXISTS /Library/Frameworks/Inventor.framework/Libraries)
#      SET(COIN_LIBRARY "/Library/Frameworks/Inventor.framework/Libraries/libCoin.dylib -F/Library/Frameworks/Inventor.framework")
      SET(COIN_LIBRARY "/Library/Frameworks/Inventor.framework")
    ENDIF (EXISTS /Library/Frameworks/Inventor.framework/Libraries)
  ENDIF (NOT COIN_LIBRARY)
ENDIF (APPLE)

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Coin DEFAULT_MSG COIN_LIBRARY COIN_INCLUDE_DIR)
