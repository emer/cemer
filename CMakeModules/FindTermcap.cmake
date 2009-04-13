# find Terrmcap (terminal input library) includes and library
#
# TERMCAP_INCLUDE_DIR - where the directory containing the TERMCAP headers can be found
# TERMCAP_LIBRARY     - full path to the TERMCAP library
# TERMCAP_FOUND       - TRUE if TERMCAP was found

FIND_PATH(TERMCAP_INCLUDE_DIR termcap.h
        /usr/include
        /usr/local/include

    /opt/local/include
    $ENV{INCLUDE}
)

FIND_LIBRARY(TERMCAP_LIBRARY NAMES termcap PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
   /usr/lib64
)

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if

# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Termcap DEFAULT_MSG TERMCAP_LIBRARY 
TERMCAP_INCLUDE_DIR)
