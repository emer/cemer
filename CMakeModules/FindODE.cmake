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
    $ENV{EMERGENTDIR}/3rdparty/include
)

FIND_LIBRARY(ODE_LIBRARY NAMES ode ode_single PATH
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    $ENV{EMERGENTDIR}/3rdparty/lib
) 

#hack because finding lib on Windows not working for some reason...
IF (WIN32)
  IF (NOT ODE_LIBRARY)
    IF (EXISTS $ENV{EMERGENTDIR}/3rdparty/lib/ode_single.lib)
      SET(ODE_LIBRARY "$ENV{EMERGENTDIR}/3rdparty/lib/ode_single.lib")
    ENDIF (EXISTS $ENV{EMERGENTDIR}/3rdparty/lib/ode_single.lib)
  ENDIF (NOT ODE_LIBRARY)
ENDIF (WIN32)

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ODE DEFAULT_MSG ODE_LIBRARY ODE_INCLUDE_DIR)
