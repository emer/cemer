# find SoQt (3d Scenegraph library) includes and library
#
# SOQT_INCLUDE_DIR - where the directory containing the SoQt headers can be found
# SOQT_LIBRARY     - full path to the SoQt library
# SOQT_FOUND       - TRUE if SoQt was found

INCLUDE_DIRECTORIES ( /Library/Frameworks/Inventor.framework )

FIND_PATH(SOQT_INCLUDE_DIR SoQt.h
        /usr/include
        /usr/local/include
	/opt/local/include
        /usr/include/Inventor/Qt
        /usr/local/include/Inventor/Qt
	/opt/local/include/Inventor/Qt
    $ENV{INCLUDE}
)

FIND_LIBRARY(SOQT_LIBRARY NAMES SoQt PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

IF (SOQT_INCLUDE_DIR AND SOQT_LIBRARY)
   SET(SOQT_FOUND TRUE)
ELSE (SOQT_INCLUDE_DIR AND SOQT_LIBRARY)
   SET(SOQT_FOUND FALSE)
ENDIF (SOQT_INCLUDE_DIR AND SOQT_LIBRARY)

IF (SOQT_FOUND)
   IF (NOT SoQt_FIND_QUIETLY)
      MESSAGE(STATUS "Found SoQt: ${SoQt_LIBRARY}")
   ENDIF (NOT SoQt_FIND_QUIETLY)
ELSE (SOQT_FOUND)
   IF (SoQt_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find SoQt. Please install it!)")
   ENDIF (SoQt_FIND_REQUIRED)
ENDIF (SOQT_FOUND)
