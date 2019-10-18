# find CCD (Open Dynamics Engine) includes and library
#
# CCD_INCLUDE_DIR - where the directory containing the CCD headers can be found
# CCD_LIBRARY     - full path to the CCD library
# CCD_FOUND       - TRUE if CCD was found

FIND_PATH(CCD_INCLUDE_DIR ccd/ccd.h
        /usr/include
        /usr/local/include
	/opt/local/include
    $ENV{INCLUDE}
    ${EMER_MISC_LIBS_DIR}/include
)

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(CCD_LIBRARY NAMES ccdd ccd_singled PATHS
      ${EMER_MISC_LIBS_DIR}/lib 
    )
  else (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(CCD_LIBRARY NAMES ccd ccd_single PATHS
      ${EMER_MISC_LIBS_DIR}/lib 
    )  
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(CCD_LIBRARY NAMES ccd ccd_single PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
  ) 
 endif (WIN32) 

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CCD DEFAULT_MSG CCD_LIBRARY CCD_INCLUDE_DIR)
