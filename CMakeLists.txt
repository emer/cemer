################################################################
# Basic Instructions for User
# 
# mkdir build;
# cd build;
# cmake ../ [-G see cmake --help for options] [-DMPI_BUILD=true] \
#  [-DCMAKE_BUILD_TYPE=None | Debug | Release | RelWithDebInfo | MinSizeRel]
#  [-DCMAKE_INSTALL_PREFIX=<path to install>]
#
# ccmake ./ to edit custom options
#
# Important notes:
# 1. do qmake -v -- if it does not say "Using Qt version 4.x.x.." (i.e., it says 3.x.x)
#    then you must find the qmake that is for Qt version 4 and set your path so it is
#    found first!

################################################################
# Step 0: ensure we have the right version of cmake -- this
# has to go directly here in this file otherwise cmake complains

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(POLICY CMP0011)
   cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

# Make 32-bit Mac build if user did:
# ./configure --emer-mac-arch-bits=32
if (EMER_MAC_ARCH_BITS EQUAL 32)
  message(STATUS "Making 32-bit Mac build")
  set(CMAKE_OSX_ARCHITECTURES "i386")
endif()

################################################################
# Step 1: set all the basic parameters about the project

# set Project -- this has important default side-effects, 
# especially on Windows, and needs to be done before almost anything else
#TODO: capital-E needed on Windows to avoid additional non-standard overrides
# ... might it not be ok for Unix/Mac too???
if (WIN32)
  PROJECT(Emergent)

  if (NOT MSVC)
    message(FATAL_ERROR "Only Microsoft Visual C++ (nmake or IDE) is supported")
  endif (NOT MSVC)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
  else ()
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
  endif ()

else (WIN32)
  PROJECT(emergent)
endif (WIN32)

# version setting
set(EMERGENT_VERSION_MAJOR "6")
set(EMERGENT_VERSION_MINOR "0")
set(EMERGENT_VERSION_PATCH "3")
set(EMERGENT_VERSION "${EMERGENT_VERSION_MAJOR}.${EMERGENT_VERSION_MINOR}.${EMERGENT_VERSION_PATCH}")

# this is used for versioning the libs themselves
set(EMERGENT_LIB_VERSION ${EMERGENT_VERSION})
set(EMERGENT_LIB_SOVERSION ${EMERGENT_VERSION_MAJOR})


################################################################
# Step 2: set or lookup various defaults, for both Emergent
#   and plugins -- this insures we have a consistent environment
include(CMakeModules/EmergentDefaults.cmake)

# use binary dir to preserve clean sep btwn src and build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


################################################################
# Step 3: misc global stuff: paths, finding packages etc
# Typically no need to change anything below here

set(mod_path "${PROJECT_SOURCE_DIR}/CMakeModules")

set(CMAKE_MODULE_PATH ${mod_path})

# turn this on for debugging new things
#set(CMAKE_VERBOSE_MAKEFILE ON)

# set the lib and executable suffix based on build type
include(${mod_path}/SetBuildSuffix.cmake)

include(CTest)

# find all of our dependencies -- also sets their include paths in include_directories
# and sets the EMERGENT_DEP_LIBRARIES variable to all the dependency libraries
find_package(EmergentDependencies)

# includes 
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/src/temt/lib
)

# install destinations
if (WIN32)
  set(EMERGENT_INCLUDE_DEST include)
  set(EMERGENT_SHARE_DEST .)
else (WIN32)
  set(EMERGENT_INCLUDE_DEST include/Emergent)
  set(EMERGENT_SHARE_DEST share/Emergent)
endif (WIN32)

# several important macros in here:
include(${mod_path}/MacroLibrary.cmake)
# this one does all the configure checks to set various variables
include(${mod_path}/ConfigureChecks.cmake)
# this one makes the config.h from cmake_config.h.in
include(${mod_path}/ConfigureGenerate.cmake)
# all the support for maketa:
include(${mod_path}/Maketa.cmake)

####################################
#  report on status prior to building

message( STATUS )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "********************* Summary of Key Build Parameters *************************" )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message( STATUS "      (Options are: Debug | Release | RelWithDebInfo | MinSizeRel)" )
if (NOT WIN32)
  message( STATUS "MPI_BUILD = ${MPI_BUILD}   (true or false)" )
endif (NOT WIN32)
message( STATUS )
message( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "********************* Summary of Prerequisities  ******************************" )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "QT_LIBRARIES = ${QT_LIBRARIES}" )
message( STATUS "COIN_LIBRARY = ${COIN_LIBRARY}" )
message( STATUS "QUARTER_LIBRARY = ${QUARTER_LIBRARY}" )
message( STATUS "GSL_LIBRARIES = ${GSL_LIBRARIES}" )
message( STATUS "ODE_LIBRARY = ${ODE_LIBRARY}" )
if (NOT WIN32)
  message( STATUS "READLINE_LIBRARY = ${READLINE_LIBRARY}" )
endif (NOT WIN32)


####################################
#  Now the actual building begins: all the subdirs

# TEMT system: The EMergent Toolkit
add_subdirectory(src/temt/maketa)
add_subdirectory(src/temt/taiqtso)
add_subdirectory(src/temt/ta)
add_subdirectory(src/temt/css)
add_subdirectory(src/temt/lib)
add_subdirectory(src/temt/css_bin)
add_subdirectory(src/temt/ta/images)

# Emergent code proper
add_subdirectory(src/emergent/network)
add_subdirectory(src/emergent/bp)
add_subdirectory(src/emergent/leabra)
add_subdirectory(src/emergent/so)
add_subdirectory(src/emergent/cs)
add_subdirectory(src/emergent/lib)
add_subdirectory(src/emergent/bin)


####################################
#  plugin commands -- use these after make install to remake plugins

set(EMERGENT_INST_EXE "emergent${EMERGENT_SUFFIX}")

add_custom_target(plugins 
  COMMAND ${EMERGENT_INST_EXE} --make_all_plugins
)

add_custom_target(user_plugins 
  COMMAND ${EMERGENT_INST_EXE} --make_all_user_plugins
)

add_custom_target(system_plugins 
  COMMAND ${EMERGENT_INST_EXE} --make_all_system_plugins
)

add_custom_target(clean_plugins 
  COMMAND ${EMERGENT_INST_EXE} --clean_all_plugins
)

add_custom_target(clean_user_plugins 
  COMMAND ${EMERGENT_INST_EXE} --clean_all_user_plugins
)

add_custom_target(clean_system_plugins 
  COMMAND ${EMERGENT_INST_EXE} --clean_all_system_plugins
)

####################################
#  plugin commands -- auto remake..

# share misc files
add_subdirectory(prog_lib)
add_subdirectory(proj_templates)
add_subdirectory(3dobj_lib)
add_subdirectory(css_lib)
add_subdirectory(test)
add_subdirectory(bench/actual)
add_subdirectory(demo/bp)
add_subdirectory(demo/cs)
add_subdirectory(demo/leabra)
add_subdirectory(demo/so)
add_subdirectory(demo/virt_env)
add_subdirectory(demo/data_proc)
add_subdirectory(data/atlases)
add_subdirectory(data/standard)

if (false) #source to make stub includes -- must be run after making the TA files
  file(GLOB_RECURSE xxx_HEADS "${PROJECT_SOURCE_DIR}/src/*.h") 
  message(STATUS "include files to be processed: ${xxx_HEADS}")
  foreach(INC_FILE ${xxx_HEADS})
    get_filename_component(DEST_FILE ${INC_FILE} NAME)
    get_filename_component(DEST_FILE_WE ${INC_FILE} NAME_WE)
    file(RELATIVE_PATH TARG_FILE "${PROJECT_SOURCE_DIR}/include" ${INC_FILE})
    CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/local_inc.h.in"
      "${PROJECT_SOURCE_DIR}/include/${DEST_FILE_WE}.h"
      IMMEDIATE @ONLY
    )
    message(STATUS "created include file for: ${INC_FILE}")
  endforeach(INC_FILE)
endif (false)

# dependencies
add_dependencies(ta_TA maketa)
add_dependencies(temt ta_TA)
add_dependencies(css temt)
#note: we have to make n_TA dep on temt otherwise VS will build all the TAs first (slow)
add_dependencies(network_TA temt)
#add_dependencies(network_TA ta_TA)
add_dependencies(bp_TA network_TA)
add_dependencies(cs_TA network_TA)
add_dependencies(leabra_TA network_TA)
add_dependencies(so_TA network_TA)
add_dependencies(emergentlib temt network_TA bp_TA cs_TA leabra_TA so_TA)
add_dependencies(emergent emergentlib)

########### install files ###############

install(FILES AUTHORS ChangeLog COPYING COPYING.LIB
  cmake_uninstall.cmake.in
  DESTINATION ${EMERGENT_SHARE_DEST}
)

# also install our cmake module files for use by plugins, etc
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules
  DESTINATION ${EMERGENT_SHARE_DEST}
  FILES_MATCHING PATTERN "*.cmake")


#plugin template files NOTE: prolly a more elegant way to make this list...
set(PLT "plugins/template")
install(FILES ${PLT}/CMakeLists.txt ${PLT}/NOBUILD ${PLT}/configure 
  ${PLT}/template_def.h
  ${PLT}/template.h ${PLT}/template.cpp
  ${PLT}/template_pl.h ${PLT}/template_pl.cpp
  ${PLT}/template_qtso.h ${PLT}/template_qtso.cpp
  DESTINATION ${EMERGENT_SHARE_DEST}/plugins/template
)

if (WIN32)
  file(GLOB OUR_3RDPARTY_FILES 
    "${EMER_3RDPARTY_DIR}/bin/*.dll"
    "${COINDIR}/bin/*.dll"
    "${QT_BINARY_DIR}/*.dll"
  )
  install(FILES ${OUR_3RDPARTY_FILES}
    DESTINATION bin
  )
  install(DIRECTORY "${EMER_3RDPARTY_DIR}"
    DESTINATION .
  )
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/temt/ta/images/emergent.ico
    DESTINATION ${EMERGENT_SHARE_DEST}
  )
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/temt/ta/images/emergent.ico
    DESTINATION bin
  )
else (WIN32)
  # plugin folder, so emergent doesn't complain
  install(DIRECTORY
    DESTINATION lib/Emergent/plugins
  )
endif (WIN32)

########### uninstall files ###############

#TODO: Uninstall Windows Registry keys
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
  
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


########### cpack packaging ###############

include(CMakeModules/EmergentCPack.cmake)

########### special targets and commands ###############

# BUG: these targets are flakily forcing all the subtargets to become ALL
# targets on Windows (at least in VS) thus we have to omit them
if (NOT WIN32)
  # use this to remove all ta files from source tree
  ADD_CUSTOM_TARGET(clean_ta_all
    DEPENDS clean_ta_ta clean_ta_network clean_ta_bp clean_ta_leabra clean_ta_cs clean_ta_so
    )

  # use this to build all ta files from source tree
  ADD_CUSTOM_TARGET(force_ta 
    DEPENDS force_ta_ta force_ta_network force_ta_bp force_ta_leabra force_ta_cs force_ta_so
    )
  # ta files are only things built in source tree so this is effectively distclean
  ADD_CUSTOM_TARGET(distclean
    DEPENDS clean_ta_all
    )
endif (NOT WIN32)

# svn commands b/c otherwise you're living in the build dir and this saves a cd..
ADD_CUSTOM_TARGET(svn_update
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND svn update
  )
ADD_CUSTOM_TARGET(svn_commit
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND svn commit
  )

