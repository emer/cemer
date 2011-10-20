# find GSL (Gnu Scientific Library) includes and library
#
# GSL_INCLUDE_DIR - where the directory containing the GSL headers can be found
# GSL_LIBRARIES   - full path to the GSL libraries (gsl and cblas)
# GSL_FOUND       - TRUE if GSL was found
# internal:
# GSL_GSL_LIBRARY   - full path to the GSL library
# GSL_CBLAS_LIBRARY - full path to the GSL CBLAS library

FIND_PATH(GSL_INCLUDE_DIR gsl/gsl_matrix.h
        /usr/include
        /usr/local/include
	/opt/local/include
    $ENV{INCLUDE}
    ${EMER_3RDPARTY_DIR}/include
    ${EMERGENTDIR}/3rdparty/include
)

# NOTE: General FIND_LIBRARY not working for some reason on Windows, but we want our own prereq
# anyways, and must use the debug version (because of runtime linkage) when in Debug anyways

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(GSL_GSL_LIBRARY NAMES gsl_d PATHS
      ${EMER_3RDPARTY_DIR}/lib 
      ${EMERGENTDIR}/3rdparty/lib
    )
    FIND_LIBRARY(GSL_CBLAS_LIBRARY NAMES cblas_d PATHS
      ${EMER_3RDPARTY_DIR}/lib 
      ${EMERGENTDIR}/3rdparty/lib
    )
  else (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(GSL_GSL_LIBRARY NAMES gsl PATHS
      ${EMER_3RDPARTY_DIR}/lib 
      ${EMERGENTDIR}/3rdparty/lib
    )  
    FIND_LIBRARY(GSL_CBLAS_LIBRARY NAMES cblas PATHS
      ${EMER_3RDPARTY_DIR}/lib 
      ${EMERGENTDIR}/3rdparty/lib
    )
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(GSL_GSL_LIBRARY NAMES gsl PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    ${EMERGENTDIR}/3rdparty/lib
  ) 

  FIND_LIBRARY(GSL_CBLAS_LIBRARY NAMES cblas gslcblas PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    $ENV{EMERGENTDIR}/3rdparty/lib
  ) 
 endif (WIN32)

SET(GSL_LIBRARIES ${GSL_GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSL DEFAULT_MSG GSL_LIBRARIES GSL_INCLUDE_DIR)
