#!/bin/bash

# note: edit package names here
qtpkg=qt593_mac64
qtdir=Qt5.9.3
vers=5.9

pkgbuild --root /usr/local/${qtdir} --install-location /usr/local/${qtdir} --identifier edu.colorado.ccnlab.emergent.qt --version ${vers} ./${qtpkg}.pkg

./make_dmg_pkg.sh $qtpkg
