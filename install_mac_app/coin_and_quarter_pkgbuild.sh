#!/bin/bash

# note: edit package names here
coinpkg=coin_mac64_4_0
qtrpkg=quarter_mac64_qt55

pkgbuild --root /Library/Frameworks/Inventor.framework --install-location /Library/Frameworks/Inventor.framework --identifier edu.colorado.ccnlab.emergent.coin --version 4.0 ./${coinpkg}.pkg

pkgbuild --root /Library/Frameworks/Quarter.framework --install-location /Library/Frameworks/Quarter.framework --identifier edu.colorado.ccnlab.emergent.quarter --version 5.5 ./${qtrpkg}.pkg

./make_dmg_pkg.sh $coinpkg
./make_dmg_pkg.sh $qtrpkg

