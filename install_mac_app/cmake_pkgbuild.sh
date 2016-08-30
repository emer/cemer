#!/bin/bash

# note: edit package names here
cmakepkg=cmake-3.6.1_mac64
cmakedir=/Applications/CMake.app
vers=3.6.1

pkgbuild --root ${cmakedir} --install-location ${cmakedir} --identifier edu.colorado.ccnlab.emergent.cmake --version ${vers} ./${cmakepkg}.pkg

./make_dmg_pkg.sh $cmakepkg
