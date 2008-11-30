# find TEMT (The Emergent Toolkit) includes and library
#
# TEMT_INCLUDE_DIR - where the directory containing the TEMT headers can be found
# TEMT_LIBRARY     - full path to the TEMT library
# TEMT_FOUND       - TRUE if TEMT was found

# IMPORTANT: the EMERGENT_SUFFIX must be set by SetBuildSuffix.cmake module prior to this!

FIND_PATH(TEMT_INCLUDE_DIR Emergent/ta_type.h
   /usr/include
   /usr/local/include
   /opt/local/include
)

FIND_LIBRARY(TEMT_LIBRARY NAMES "temt${EMERGENT_SUFFIX}" PATH
  /usr/lib
  /usr/local/lib
  /opt/local/lib
)

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Temt DEFAULT_MSG TEMT_LIBRARY TEMT_INCLUDE_DIR)
