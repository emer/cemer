################################################################
# This is the magic master configure for emergent plugins
# they find the share dir and load this file, and it sets up
# everything else needed to compile a plugin off of the installed
# emergent code

# IMPORTANT: EMERGENT_SHARE_DIR will be set by the plugin CMakeLists.txt 
# file, so we can use that to then find other stuff etc

set(mod_path "${EMERGENT_SHARE_DIR}/Emergent/CMakeModules")

set(CMAKE_MODULE_PATH ${mod_path})

# set the lib and executable suffix based on build type -- need this before finding
# the emergent and temt packages!
include(${mod_path}/SetBuildSuffix.cmake)

# find full emergent install
find_package(Emergent)
include_directories(${EMERGENT_INCLUDE_DIR}/Emergent)

# find all of our dependencies -- also sets their include paths in include_directories
# and sets the EMERGENT_DEP_LIBRARIES variable to all the dependency libraries
find_package(EmergentDependencies)

set(EMERGENT_PLUGIN_DEST lib/Emergent/plugins)

# several important macros in here:
include(${mod_path}/MacroLibrary.cmake)
# this one does all the configure checks to set various variables
include(${mod_path}/ConfigureChecks.cmake)
# all the support for maketa:
include(${mod_path}/Maketa.cmake)

# use this instead of TARGET_LINK_LIBRARIES -- sets suffix and other properties
MACRO (EMERGENT_PLUGIN_LINK_LIBRARIES _targ _xtra_libs)
    target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_LIBRARIES} ${EMERGENT_DEP_LIBRARIES})
    set_target_properties(${_targ} PROPERTIES
      VERSION ${PLUGIN_VERSION}
      SOVERSION ${PLUGIN_VERSION_MAJOR}
    )
ENDMACRO (EMERGENT_PLUGIN_LINK_LIBRARIES)

####################################
#  report on status prior to building

message( STATUS )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "********************* Summary of Key Build Parameters *************************" )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message( STATUS "      (Options are: None | Debug | Release | RelWithDebInfo | MinSizeRel)" )
message( STATUS "MPI_BUILD = ${MPI_BUILD}   (true or false)" )
message( STATUS )
message( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
message( STATUS "-------------------------------------------------------------------------------" )
