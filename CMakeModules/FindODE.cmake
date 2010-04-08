# find ODE (Open Dynamics Engine) includes and library
#
# ODE_INCLUDE_DIR - where the directory containing the ODE headers can be found
# ODE_LIBRARY     - full path to the ODE library
# ODE_FOUND       - TRUE if ODE was found

FIND_PATH(ODE_INCLUDE_DIR ode/ode.h
        /usr/include
        /usr/local/include
	/opt/local/include
    $ENV{INCLUDE}
    ${PROJECT_SOURCE_DIR}/3rdparty/include
    ${EMERGENTDIR}/3rdparty/include
)

if (WIN32)
  if (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(ODE_LIBRARY NAMES ode_singled PATHS
      ${PROJECT_SOURCE_DIR}/3rdparty/lib 
      ${EMERGENTDIR}/3rdparty/lib
    )
  else (CMAKE_BUILD_TYPE MATCHES "Debug")
    FIND_LIBRARY(ODE_LIBRARY NAMES ode_single PATHS
      ${PROJECT_SOURCE_DIR}/3rdparty/lib 
      ${EMERGENTDIR}/3rdparty/lib
    )  
  endif (CMAKE_BUILD_TYPE MATCHES "Debug")
else (WIN32)
  FIND_LIBRARY(ODE_LIBRARY NAMES ode ode_single PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
  ) 
 endif (WIN32) 

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ODE DEFAULT_MSG ODE_LIBRARY ODE_INCLUDE_DIR)
