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

if (WIN32)
  # There is a bug in NSIS that does not handle full unix paths properly.
  # Make sure there is at least one set of four (4) backslashes.
  
  # Set the name of the installer executable to be created.
  SET(CPACK_PACKAGE_FILE_NAME "emergent-${EMERGENT_VERSION}-win32")
  
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
  SET(CPACK_CREATE_DESKTOP_LINKS emergent)
  
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
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "subversion, cmake, g++, libqt4-dev, libcoin60-dev, libreadline6-dev, libgsl0-dev, zlib1g-dev, libode-sp-dev, libpng-dev, libjpeg8-dev, libquarter")
  endif (APPLE)
endif (WIN32)


SET(CPACK_PACKAGE_EXECUTABLES "emergent" "Emergent")
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
