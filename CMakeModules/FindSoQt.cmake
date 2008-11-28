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
    $ENV{INCLUDE}
)

FIND_LIBRARY(SOQT_LIBRARY NAMES SoQt PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SoQt DEFAULT_MSG SOQT_LIBRARY SOQT_INCLUDE_DIR)
