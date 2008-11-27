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
)

FIND_LIBRARY(ODE_LIBRARY NAMES ode PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

IF (ODE_INCLUDE_DIR AND ODE_LIBRARY)
   SET(ODE_FOUND TRUE)
ELSE (ODE_INCLUDE_DIR AND ODE_LIBRARY)
   SET(ODE_FOUND FALSE)
ENDIF (ODE_INCLUDE_DIR AND ODE_LIBRARY)

IF (ODE_FOUND)
   IF (NOT ODE_FIND_QUIETLY)
      MESSAGE(STATUS "Found ODE: ${ODE_LIBRARY}")
   ENDIF (NOT ODE_FIND_QUIETLY)
ELSE (ODE_FOUND)
   IF (ODE_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find ODE. Please install ODE (http://www.ode.org)")
   ENDIF (ODE_FIND_REQUIRED)
ENDIF (ODE_FOUND)
