# find GSL (Gnu Scientific Library) includes and library
#
# GSL_INCLUDE_DIR - where the directory containing the GSL headers can be found
# GSL_LIBRARY     - full path to the GSL library
# GSL_FOUND       - TRUE if GSL was found

FIND_PATH(GSL_INCLUDE_DIR gsl/gsl_matrix.h
        /usr/include
        /usr/local/include
	/opt/local/include
    $ENV{INCLUDE}
)

FIND_LIBRARY(GSL_LIBRARY NAMES gsl PATH
   /usr/lib
   /usr/local/lib
   /opt/local/lib
) 

# handle the QUIETLY and REQUIRED arguments and set COIN_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSL DEFAULT_MSG GSL_LIBRARY GSL_INCLUDE_DIR)
