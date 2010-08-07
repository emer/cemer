################################################################
# this actually defines a standard set of make targets for building the software
# using the macros defined in the EmergentPlugin.cmake include
# If you have more advanced needs, you can instead look at this file
# and copy the commands in here instead and modify them to suit your needs.

QT4_WRAP_CPP(plugin_SRCS_MOC ${plugin_HEADS_MOC})

# this sets up system to scan header files for type information using maketa
CREATE_MAKETA_COMMAND(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}" "${plugin_HEADS_MAKETA}")

# set the full set of sources including auto-built ones here -- important that
# _TA.cpp file is first one to make sure maketa is run first
set(plugin_full_SRCS
  ${PROJECT_NAME}_TA.cpp ${plugin_SRCS}
  ${plugin_SRCS_MOC}
)

# adds the library as an official target to compile
add_library(${PROJECT_NAME} SHARED ${plugin_full_SRCS})

# dependencies
add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_TA")

# does all the stuff to make the library link against all the right other libraries
# final arg is for any extra libraries this plugin might want
EMERGENT_PLUGIN_LINK_LIBRARIES(${PROJECT_NAME} 
  "${EMERGENT_PLUGIN_EXTRA_LIBRARIES}")


####################################
#  report on status after building

message( STATUS )
message( STATUS "----------------------------------------------------------------------------" )
message( STATUS "******************* Summary of Key Build Parameters ************************" )
message( STATUS "----------------------------------------------------------------------------" )
message( STATUS "(Default values indicated by *" )
message( STATUS "EMERGENT_PLUGIN_TYPE = ${EMERGENT_PLUGIN_TYPE}" )
message( STATUS "    (Options are: User | System)" )
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

set(MY_FULL_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/${EMERGENT_PLUGIN_DEST}/${PROJECT_NAME}")

if (NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${MY_FULL_INSTALL_PATH}")
  file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/orig_src_path.txt" "${CMAKE_CURRENT_SOURCE_DIR}")
endif (NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${MY_FULL_INSTALL_PATH}")

################################################################
# Step 5: install/uninstall stuff -- only for non-default

install(FILES ${plugin_FILES} "PluginWizard.wiz" "orig_src_path.txt"
  DESTINATION ${EMERGENT_PLUGIN_DEST}/${PROJECT_NAME}
  )

if (WIN32)
  install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${EMERGENT_PLUGIN_DEST}
    )
else (WIN32)
  # note: we don't support linking to plugins, so no ARCHIVE dest
  install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${EMERGENT_PLUGIN_DEST}
    )
endif (WIN32)

########### uninstall files ###############
CONFIGURE_FILE(
  "${EMERGENT_SHARE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
