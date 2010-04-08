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
)

FIND_LIBRARY(SNDFILE_LIBRARY NAMES sndfile PATHS
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sndfile DEFAULT_MSG SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR)
