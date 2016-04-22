#!/bin/bash

pkgbuild --root /Library/Frameworks/Inventor.framework --identifier coin.emergent.ccnlab.colorado.edu --version 4.0 ./coin_mac64_4_0.pkg

pkgbuild --root /Library/Frameworks/Quarter.framework/ --identifier quarter.emergent.ccnlab.colorado.edu --version 5.5 ./quarter_mac64_qt55.pkg

#pkgbuild --component /Library/Frameworks/Inventor.framework/ --component /Library/Frameworks/Quarter.framework/ --identifier coin.quarter.emergent.ccnlab.colorado.edu --version 5.5 ./coin_and_quarter_mac64_qt55.pkg

# doesn't like the coin-config -- probably need a script or something.
#pkgbuild --component /Library/Frameworks/Inventor.framework --component /Library/Frameworks/Quarter.framework --component /usr/local/bin/coin-config --identifier coin.quarter.emergent.ccnlab.colorado.edu --version 5.5 --install-location / ./coin_and_quarter_mac64_qt55.pkg

