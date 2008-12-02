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
    $ENV{COINDIR}/include/Inventor/Qt
)
#MESSAGE("SOQT_INCLUDE_DIR=" ${SOQT_INCLUDE_DIR})

FIND_LIBRARY(SOQT_LIBRARY NAMES SoQt PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

#hack because finding lib on Windows not working for some reason...
IF (WIN32)
  IF (NOT SOQT_LIBRARY)
    IF (EXISTS $ENV{COINDIR}/lib/soqt1.lib)
      SET(SOQT_LIBRARY "$ENV{COINDIR}/lib/soqt1.lib")
    ENDIF (EXISTS $ENV{COINDIR}/lib/soqt1.lib)
  ENDIF (NOT SOQT_LIBRARY)
ENDIF (WIN32)

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SoQt DEFAULT_MSG SOQT_LIBRARY SOQT_INCLUDE_DIR)
