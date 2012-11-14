# This file contains all of our cpack configuration information, which is then generated out to CPackConfig.cmake
# DO NOT EDIT CPackConfig.cmake directly!!!

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Emergent Neural Simulation System")
SET(CPACK_PACKAGE_VENDOR "Computational Cognitive Neuroscience Lab, University of Colorado at Boulder")
SET(CPACK_PACKAGE_CONTACT "emergent-users@grey.colorado.edu")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "${EMERGENT_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${EMERGENT_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${EMERGENT_VERSION_PATCH}")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(EMER_ARCH_BITS "64")
else ()
  SET(EMER_ARCH_BITS "32")
endif ()

if (WIN32)
  # There is a bug in NSIS that does not handle full unix paths properly.
  # Make sure there is at least one set of four (4) backslashes.
  
  # Set the name of the installer executable to be created.
  SET(CPACK_PACKAGE_FILE_NAME "emergent${EMERGENT_SUFFIX}-${EMERGENT_VERSION}-win${EMER_ARCH_BITS}")
  
  # Set the icon used for the installer (and uninstaller).
  SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/temt/ta/images\\\\emergent.ico")
  SET(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/src/temt/ta/images\\\\emergent.ico")
  
  # Set a graphic to use in the installer (not an icon).
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/temt/ta/images\\\\emergent-install.bmp")
  
  # Set the icon used in the Add/Remove Programs control panel.
  # The path has to be relative to the install directory, not
  # the build/source directory.  The file has to be an .exe.
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\emergent${EMERGENT_SUFFIX}.exe")
  
  # Have the installer prompt the user with the "Install Options" screen,
  # which has prompts to modify the system path and create desktop shortcuts.
  SET(CPACK_NSIS_MODIFY_PATH ON)
  
  # Set the list of desktop shortcuts to create, if the user so chooses.
  # However, if the user later selects "Do not create shortcuts" on the
  # "Choose Start Menu Folder" screen, it not only omits the SM shortcuts,
  # but also skips the desktop shortcut.  This is probably a bug in NSIS.
  #
  # Has to be lowercase "emergent", not "Emergent" to match what's in
  # CPACK_PACKAGE_EXECUTABLES.
  SET(CPACK_CREATE_DESKTOP_LINKS emergent${EMERGENT_SUFFIX})
  
else (WIN32)
  # Replace CMAKE_INSTALL_PREFIX to Currently used one,
  # If it wasn't overridden from command line / cache.
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/temt/ta/images/emergent_32x32.png")
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
  endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
  if(APPLE)
    SET(CPACK_GENERATOR "PackageMaker")
    SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/temt/ta/images/emergent_32x32.png")
    SET(CPACK_PACKAGE_FILE_NAME "emergent-${EMERGENT_VERSION}-mac")
  else (APPLE)
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "subversion, cmake, g++, libqt4-dev, libcoin60-dev, libreadline6-dev, libgsl0-dev, zlib1g-dev, libode-sp-dev, libpng-dev, libjpeg-dev, libquarter, libncurses-dev, libsvn-dev")
    # CPACK_DEBIAN_PACKAGE_ARCHITECTURE should get set automatically by dpkg --print-architecture
    # ... except that doesn't happen until CPack runs, and we need it here to set the package filename
    # This is copied from CPackDeb.cmake.
    FIND_PROGRAM(DPKG_CMD dpkg)
    IF(NOT DPKG_CMD)
      MESSAGE(STATUS "Can not find dpkg in your path, default to i386.")
      SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
    ENDIF(NOT DPKG_CMD)
    EXECUTE_PROCESS(COMMAND "${DPKG_CMD}" --print-architecture
      OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    SET(CPACK_PACKAGE_FILE_NAME "emergent-${EMERGENT_VERSION}-Linux-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/debian/postrm;")
  endif (APPLE)
endif (WIN32)

if (WIN32)
  # Normally these are all derived from the CPACK_PACKAGE_NAME, but
  # it looks funny when the version number gets appended after the
  # architecture, so instead set everything explicitly.
  SET(EMER_FULL_NAME "Emergent${EMERGENT_SUFFIX} ${EMERGENT_VERSION} (${EMER_ARCH_BITS}-bit)")
  SET(CPACK_NSIS_DISPLAY_NAME "${EMER_FULL_NAME}")
  SET(CPACK_NSIS_PACKAGE_NAME "${EMER_FULL_NAME}")
  SET(CPACK_PACKAGE_EXECUTABLES "emergent${EMERGENT_SUFFIX};${EMER_FULL_NAME}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${EMER_FULL_NAME}")
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${EMER_FULL_NAME}")
  SET(CPACK_PACKAGE_NAME "${EMER_FULL_NAME}")
else (WIN32)
  SET(CPACK_PACKAGE_EXECUTABLES "emergent" "Emergent")
endif (WIN32)

# Don't do this.  Including system libraries isn't allowed by GPL v3.
# Currently, emergent is "GPL v2 or later".  There's no reason to make
# it difficult to upgrade later on if we want to.
# # Include the MSVC redistributable package in the installer.
# INCLUDE(InstallRequiredSystemLibraries)

INCLUDE(CPack)

# following are currently unused
#SET(CPACK_BINARY_BUNDLE "")
#SET(CPACK_BINARY_CYGWIN "")
#SET(CPACK_BINARY_DEB "ON")
#SET(CPACK_BINARY_NSIS "OFF")
#SET(CPACK_BINARY_OSXX11 "")
#SET(CPACK_BINARY_PACKAGEMAKER "")
#SET(CPACK_BINARY_RPM "OFF")
#SET(CPACK_BINARY_STGZ "OFF")
#SET(CPACK_BINARY_TBZ2 "OFF")
#SET(CPACK_BINARY_TGZ "OFF")
#SET(CPACK_BINARY_TZ "OFF")
#SET(CPACK_BINARY_ZIP "OFF")
#SET(CPACK_COMPONENTS_ALL "")
#SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
#SET(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
