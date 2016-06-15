#!/bin/bash

# note: edit package names here
coinpkg=coin_mac64_4_0

pkgbuild --root /Library/Frameworks/Inventor.framework --install-location /Library/Frameworks/Inventor.framework --identifier edu.colorado.ccnlab.emergent.coin --version 4.0 ./${coinpkg}.pkg

./make_dmg_pkg.sh $coinpkg

