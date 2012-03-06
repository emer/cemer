# set the lib and executable suffix based on build type
# CMAKE_DEBUG_POSTFIX: libraries
# CMAKE_EXECUTABLE_SUFFIX: executables
# also sets global EMERGENT_SUFFIX for general use
# (note location of "" -- needed around entire otherwise they are treated like a list with ;)
# note: for XCode/VS all target types are available, so need to make them all
if (MPI_BUILD)
  set(CMAKE_DEBUG_POSTFIX "${EXTRA_SUFFIX}_dbg_mpi")
  set(CMAKE_RELEASE_POSTFIX "${EXTRA_SUFFIX}_mpi")
  set(CMAKE_RELWITHDEBINFO_POSTFIX "${EXTRA_SUFFIX}_mpi")
  set(CMAKE_MINSIZEREL_POSTFIX "${EXTRA_SUFFIX}_mpi")
else (MPI_BUILD)
  set(CMAKE_DEBUG_POSTFIX "${EXTRA_SUFFIX}_dbg")
  set(CMAKE_RELEASE_POSTFIX "${EXTRA_SUFFIX}")
  set(CMAKE_RELWITHDEBINFO_POSTFIX "${EXTRA_SUFFIX}")
  set(CMAKE_MINSIZEREL_POSTFIX "${EXTRA_SUFFIX}")
endif (MPI_BUILD)
# TODO: need to make all this work in all contexts!
if (CMAKE_BUILD_TYPE MATCHES "Debug") 
  if (MPI_BUILD)
	Set(EMERGENT_SUFFIX "${EXTRA_SUFFIX}_dbg_mpi")
  else (MPI_BUILD)
	set(EMERGENT_SUFFIX "${EXTRA_SUFFIX}_dbg")
  endif (MPI_BUILD)
else (CMAKE_BUILD_TYPE MATCHES "Debug") 
  if (MPI_BUILD)
	set(EMERGENT_SUFFIX "${EXTRA_SUFFIX}_mpi")
  else(MPI_BUILD)
	set(EMERGENT_SUFFIX "${EXTRA_SUFFIX}")
  endif (MPI_BUILD)
endif (CMAKE_BUILD_TYPE MATCHES "Debug") 
