################################################################
# This is the magic master configure for emergent plugins
# they find the share dir and load this file, and it sets up
# everything else needed to compile a plugin off of the installed
# emergent code

####################################
#  report on status prior to building

message( STATUS )
message( STATUS "----------------------------------------------------------------------------" )
message( STATUS "******************* Summary of Key Build Parameters ************************" )
message( STATUS "----------------------------------------------------------------------------" )
message( STATUS "(Default values indicated by *" )
message( STATUS "EMERGENT_PLUGIN_TYPE = ${EMERGENT_PLUGIN_TYPE}" )
message( STATUS "    (Options are: User | System | Default)" )
file(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}/${EMERGENT_PLUGIN_DEST}"
  EMERGENT_PLUGIN_INSTALL_NATIVE)
message( STATUS "Installation will be to: " ${EMERGENT_PLUGIN_INSTALL_NATIVE} )
message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message( STATUS "    (Options are: Debug | Release | RelWithDebInfo *)" )
if (NOT WIN32)
message( STATUS "MPI_BUILD = ${MPI_BUILD}   (true or false)" )
endif (NOT WIN32)
message( STATUS)
message( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
message( STATUS "----------------------------------------------------------------------------" )


