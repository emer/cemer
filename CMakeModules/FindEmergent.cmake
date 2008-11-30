# find EMERGENT (Emergent Neural Network Simulation System) includes and library
#
# EMERGENT_INCLUDE_DIR - where the directory containing the EMERGENT headers can be found
# EMERGENT_LIBRARY     - full path to the EMERGENT library
# EMERGENT_LIBRARIES   - both the temt and emergent libraries
# EMERGENT_FOUND       - TRUE if EMERGENT was found

# IMPORTANT: the EMERGENT_SUFFIX must be set by SetBuildSuffix.cmake module prior to this!

# first find temt -- emergent requires that, and sets EMERGENT_LIBRARIES
find_package(Temt REQUIRED)

FIND_PATH(EMERGENT_INCLUDE_DIR Emergent/ta_type.h
   /usr/include
   /usr/local/include
   /opt/local/include
)

FIND_LIBRARY(EMERGENT_LIBRARY NAMES emergentlib${EMERGENT_SUFFIX} PATH
  /usr/lib
  /usr/local/lib
  /opt/local/lib
)

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Emergent DEFAULT_MSG EMERGENT_LIBRARY EMERGENT_INCLUDE_DIR)

set(EMERGENT_LIBRARIES ${TEMT_LIBRARY} ${EMERGENT_LIBRARY})
