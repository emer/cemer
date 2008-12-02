# find Readline (terminal input library) includes and library
#
# READLINE_INCLUDE_DIR - where the directory containing the READLINE headers can be found
# READLINE_LIBRARY     - full path to the READLINE library
# READLINE_FOUND       - TRUE if READLINE was found
IF (NOT WIN32)
FIND_PATH(READLINE_INCLUDE_DIR readline/readline.h
        /usr/include
        /usr/local/include
	/opt/local/include
    $ENV{INCLUDE}
)

FIND_LIBRARY(READLINE_LIBRARY NAMES readline PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Readline DEFAULT_MSG READLINE_LIBRARY READLINE_INCLUDE_DIR)
ENDIF (NOT WIN32)