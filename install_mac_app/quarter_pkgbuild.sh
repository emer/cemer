#!/bin/bash

# note: edit package names here
qtrpkg=quarter_mac64_qt56

pkgbuild --root /Library/Frameworks/Quarter.framework --install-location /Library/Frameworks/Quarter.framework --identifier edu.colorado.ccnlab.emergent.quarter --version 5.5 ./${qtrpkg}.pkg

./make_dmg_pkg.sh $qtrpkg
