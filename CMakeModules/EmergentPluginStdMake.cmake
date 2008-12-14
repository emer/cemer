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

add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_TA")

# does all the stuff to make the library link against all the right other libraries
# final arg is for any extra libraries this plugin might want
EMERGENT_PLUGIN_LINK_LIBRARIES(${PROJECT_NAME} 
  "${EMERGENT_PLUGIN_EXTRA_LIBRARIES}")

################################################################
# Step 5: install stuff

install(FILES ${plugin_FILES}
  DESTINATION ${EMERGENT_PLUGIN_DEST}/${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${EMERGENT_PLUGIN_DEST}
)

########### uninstall files ###############

CONFIGURE_FILE(
  "${EMERGENT_SHARE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
