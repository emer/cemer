# find TEMT (The Emergent Toolkit) includes and library
#
# TEMT_INCLUDE_DIR - where the directory containing the TEMT headers can be found
# TEMT_LIBRARY     - full path to the TEMT library
# TEMT_FOUND       - TRUE if TEMT was found

FIND_PATH(TEMT_INCLUDE_DIR ta_type.h
   /usr/include/Emergent
   /usr/local/include/Emergent
   /opt/local/include/Emergent
   /usr/include/emergent
   /usr/local/include/emergent
   /opt/local/include/emergent
)

IF (MPI_BUILD)
  FIND_LIBRARY(TEMT_LIBRARY NAMES temt_mpi temt_mpi-4.0.14 PATH
     /usr/lib
     /usr/local/lib
     /opt/local/lib
  )
ELSE (MPI_BUILD)
  FIND_LIBRARY(TEMT_LIBRARY NAMES temt temt-4.0.14 PATH
     /usr/lib
     /usr/local/lib
     /opt/local/lib
  )
ENDIF (MPI_BUILD)

IF (TEMT_INCLUDE_DIR AND TEMT_LIBRARY)
   SET(TEMT_FOUND TRUE)
ELSE (TEMT_INCLUDE_DIR AND TEMT_LIBRARY)
   SET(TEMT_FOUND FALSE)
ENDIF (TEMT_INCLUDE_DIR AND TEMT_LIBRARY)

IF (TEMT_FOUND)
   IF (NOT Temt_FIND_QUIETLY)
      MESSAGE(STATUS "Found Temt: ${TEMT_LIBRARY}")
   ENDIF (NOT Temt_FIND_QUIETLY)
ELSE (TEMT_FOUND)
   IF (Temt_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Temt")
   ENDIF (Temt_FIND_REQUIRED)
ENDIF (TEMT_FOUND)
