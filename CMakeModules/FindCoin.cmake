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
    $ENV{COINDIR}/include/Inventor
)

# NOTE: it is not finding the coin framework because the name is Inventor but the lib is Coin!
# SoQt works perfectly just right out of the bag

FIND_LIBRARY(COIN_LIBRARY NAMES Coin PATH
   C:/Coin-2.5.0/lib
   /usr/lib
   /usr/local/lib
   /opt/local/lib
   /Library/Frameworks/Inventor.framework/Libraries
   $ENV{COINDIR}/lib
) 
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

#hack because finding lib on Windows not working for some reason...
IF (WIN32)
  IF (NOT COIN_LIBRARY)
    IF (EXISTS $ENV{COINDIR}/lib/coin2.lib)
      SET(COIN_LIBRARY "$ENV{COINDIR}/lib/coin2.lib")
    ENDIF (EXISTS $ENV{COINDIR}/lib/coin2.lib)
  ENDIF (NOT COIN_LIBRARY)
ENDIF (WIN32)
#MESSAGE("COIN_LIBRARY=" ${COIN_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Coin DEFAULT_MSG COIN_LIBRARY COIN_INCLUDE_DIR)
