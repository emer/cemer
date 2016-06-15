#!/bin/bash

# note: edit package names here
qtpkg=qt561_mac64
qtdir=Qt5.6.1
vers=5.6

pkgbuild --root /usr/local/${qtdir} --install-location /usr/local/${qtdir} --identifier edu.colorado.ccnlab.emergent.qt --version ${vers} ./${qtpkg}.pkg

./make_dmg_pkg.sh $qtpkg
