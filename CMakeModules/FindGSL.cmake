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
    ${EMERGENTDIR}/3rdparty/include
)

# NOTE: General FIND_LIBRARY not working for some reason on Windows, but we want our own prereq
# anyways, and must use the debug version (because of runtime linkage) when in Debug anyways

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    IF (EXISTS ${EMERGENTDIR}/3rdparty/lib/gsl_d.lib)
      SET(GSL_GSL_LIBRARY "${EMERGENTDIR}/3rdparty/lib/gsl_d.lib")
    ENDIF (EXISTS ${EMERGENTDIR}/3rdparty/lib/gsl_d.lib)
    IF (EXISTS ${EMERGENTDIR}/3rdparty/lib/cblas_d.lib)
      SET(GSL_CBLAS_LIBRARY "${EMERGENTDIR}/3rdparty/lib/cblas_d.lib")
    ENDIF (EXISTS ${EMERGENTDIR}/3rdparty/lib/cblas_d.lib)
  else (CMAKE_BUILD_TYPE MATCHES "Debug") 
    IF (EXISTS ${EMERGENTDIR}/3rdparty/lib/gsl.lib)
      SET(GSL_GSL_LIBRARY "${EMERGENTDIR}/3rdparty/lib/gsl.lib")
    ENDIF (EXISTS ${EMERGENTDIR}/3rdparty/lib/gsl.lib)
    IF (EXISTS ${EMERGENTDIR}/3rdparty/lib/cblas.lib)
      SET(GSL_CBLAS_LIBRARY "${EMERGENTDIR}/3rdparty/lib/cblas.lib")
    ENDIF (EXISTS ${EMERGENTDIR}/3rdparty/lib/cblas.lib)
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(GSL_GSL_LIBRARY NAMES gsl PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    ${EMERGENTDIR}/3rdparty/lib
  ) 

  FIND_LIBRARY(GSL_CBLAS_LIBRARY NAMES cblas gslcblas PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    $ENV{EMERGENTDIR}/3rdparty/lib
  ) 
 endif (WIN32) 

#hack because finding lib on Windows not working for some reason...
IF (WIN32)
  IF (NOT GSL_GSL_LIBRARY)
    IF (EXISTS $ENV{EMERGENTDIR}/3rdparty/lib/gsl.lib)
      SET(GSL_GSL_LIBRARY "$ENV{EMERGENTDIR}/3rdparty/lib/gsl.lib")
    ENDIF (EXISTS $ENV{EMERGENTDIR}/3rdparty/lib/gsl.lib)
  ENDIF (NOT GSL_GSL_LIBRARY)
  IF (NOT GSL_CBLAS_LIBRARY)
    IF (EXISTS $ENV{EMERGENTDIR}/3rdparty/lib/cblas.lib)
      SET(GSL_CBLAS_LIBRARY "$ENV{EMERGENTDIR}/3rdparty/lib/cblas.lib")
    ENDIF (EXISTS $ENV{EMERGENTDIR}/3rdparty/lib/cblas.lib)
  ENDIF (NOT GSL_CBLAS_LIBRARY)
ENDIF (WIN32)

SET(GSL_LIBRARIES ${GSL_CBLAS_LIBRARY} ${GSL_GSL_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSL DEFAULT_MSG GSL_LIBRARIES GSL_INCLUDE_DIR)
