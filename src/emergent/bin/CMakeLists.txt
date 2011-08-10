########### emergent bin target ###############

set(emergentbin_SRCS main.cpp)

# Add icon resource for window builds
if (WIN32)
  set(emergentbin_SRCS ${emergentbin_SRCS} icon.rc)
endif (WIN32)

add_executable(emergent ${emergentbin_SRCS})

EMERGENT_LINK_EXECUTABLES(emergent emergentlib temt)

# Windows import/export control
if (WIN32)
  add_definitions(-DTA_DLL -DEMERGENT_DLL)
endif (WIN32)

########### install files ###############

install(TARGETS emergent
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY ${QT_PLUGINS_DIR}/imageformats
  DESTINATION bin
  USE_SOURCE_PERMISSIONS
)

# Add a desktop icon for Linux
if (UNIX AND NOT APPLE)
  set (DESKTOP_ENTRY emergent.desktop)
  add_custom_command(OUTPUT ${DESKTOP_ENTRY}
    COMMAND touch ${DESKTOP_ENTRY}
    COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/emergent-desktop.sh ${CMAKE_INSTALL_PREFIX} > ${DESKTOP_ENTRY}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/emergent-desktop.sh
    COMMENT "Generating desktop entry file"
  )
  add_custom_target(desktop_entry_file ALL DEPENDS ${DESKTOP_ENTRY})
  add_dependencies(emergent desktop_entry_file)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_ENTRY} DESTINATION /usr/share/applications/)
endif (UNIX AND NOT APPLE)

