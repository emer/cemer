# find Coin (3d Scenegraph library) includes and library
#
# COIN_INCLUDE_DIR - where the directory containing the Coin headers can be found
# COIN_LIBRARY     - full path to the Coin library
# COIN_FOUND       - TRUE if Coin was found

INCLUDE_DIRECTORIES ( /Library/Frameworks/Inventor.framework )

FIND_PATH(COIN_INCLUDE_DIR SbVec3f.h
        /usr/include
        /usr/local/include
	/opt/local/include
        /usr/include/Inventor
        /usr/local/include/Inventor
	/opt/local/include/Inventor
    $ENV{INCLUDE}
)

FIND_LIBRARY(COIN_LIBRARY NAMES Coin PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

IF (COIN_INCLUDE_DIR AND COIN_LIBRARY)
   SET(COIN_FOUND TRUE)
ELSE (COIN_INCLUDE_DIR AND COIN_LIBRARY)
   SET(COIN_FOUND FALSE)
ENDIF (COIN_INCLUDE_DIR AND COIN_LIBRARY)

IF (COIN_FOUND)
   IF (NOT Coin_FIND_QUIETLY)
      MESSAGE(STATUS "Found Coin: ${Coin_LIBRARY}")
   ENDIF (NOT Coin_FIND_QUIETLY)
ELSE (COIN_FOUND)
   IF (Coin_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Coin. Please install it!)")
   ENDIF (Coin_FIND_REQUIRED)
ENDIF (COIN_FOUND)
