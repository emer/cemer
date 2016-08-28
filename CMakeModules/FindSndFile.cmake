# find Sndfile (audio sound file processing library) includes and library
#
# SNDFILE_INCLUDE_DIR - where the directory containing the SNDFILE headers can be found
# SNDFILE_LIBRARY     - full path to the SNDFILE library
# SNDFILE_FOUND       - TRUE if SNDFILE was found

FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.hh
        /usr/include
        /usr/local/include
	/opt/local/include
    $ENV{INCLUDE}
    ${EMER_MISC_LIBS_DIR}/include
) 

if (WIN32)
  FIND_LIBRARY(SNDFILE_LIBRARY NAMES libsndfile-1 PATHS
    ${EMER_MISC_LIBS_DIR}/lib
  )
else (WIN32)
  FIND_LIBRARY(SNDFILE_LIBRARY NAMES sndfile PATHS
   /usr/lib
   /usr/local/lib
   /opt/local/lib
   ${EMER_MISC_LIBS_DIR}/lib
  )
endif (WIN32)

# handle the QUIETLY and REQUIRED arguments and set x_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sndfile DEFAULT_MSG SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR)
