#!/bin/bash

# install emergent mac dependencies script
# see http://grey.colorado.edu/emergent

# this should be the only thing you need to update: grab the latest versions
QT_DMG=qt-mac-opensource-4.8.4.dmg
COIN_DMG=Coin-4.0.0a.dmg
QUARTER_DMG=quarter_mac64_qt4.dmg
CMAKE_DMG=cmake-2.8.10.2-Darwin64-universal.dmg
SVN_DMG=Subversion-1.7.8_10.8.x.dmg
MISC_DMG=emergent_misc_deps_mac64.dmg
 
FTP_REPO=ftp://grey.colorado.edu/pub/emergent
FTP_CMD="/usr/bin/ftp -ai"

DOWNLOAD_DIR=$HOME/Desktop
 
if [[ ! -d $DOWNLOAD_DIR ]]
then
  mkdir $DOWNLOAD_DIR
fi

cd $DOWNLOAD_DIR

function downloadFTP {
  # ARG is file to download - goes to download dir, checks for existence already
  if [ ! -e "$1" ]; then
      echo "downloading file: $1"
      ${FTP_CMD} "${FTP_REPO}/$1"
  fi
}

downloadFTP ${MISC_DMG}
downloadFTP ${QUARTER_DMG}
downloadFTP ${COIN_DMG}
downloadFTP ${CMAKE_DMG}
downloadFTP ${SVN_DMG}
downloadFTP ${QT_DMG}

function mountDMG {
  # Argument should be the name of the DMG.
  # Outputs the mount directory.

  # Note: there are tabs in the line below!  For some dumb reason the
  # output of hdiutil is a mix of tabs and spaces to separate columns --
  # not very friendly to scripts!
  hdiutil attach "$1" | sed -n 's/^\/dev.*[ 	]Apple_HFS[ 	]*//p'
}

function unmountDMG {
  # Argument should be the directory the DMG was mounted to.
  hdiutil detach "$1"
}

function openPKGinDMG {
  # Argument should be the name of the DMG.
  # finds the package file, opens it

  DMG_MNT="`mountDMG $1`"
  echo "mounted: $DMG_MNT"
  DMG_PKG="`ls -1rtd \"$DMG_MNT\"/*pkg | tail -1`"
  echo "package: $DMG_PKG"
  open "${DMG_PKG}"
}

function openNamedPKGinDMG {
  # Argument 1 should be the name of the DMG.
  # Argument 2 is name of the package
  # finds the package file, opens it

  DMG_MNT="`mountDMG $1`"
  echo "mounted: $DMG_MNT"
  DMG_PKG="`ls -1rtd \"$DMG_MNT\"/$2 | tail -1`"
  echo "package: $DMG_PKG"
  open "${DMG_PKG}"
}

# these are listed in REVERSE order of size and dependency, independent to dependent
# QUARTER depends on COIN, QT
# everything else is independent
# all are opened so last is first user will see
openPKGinDMG ${QUARTER_DMG}
openPKGinDMG ${QT_DMG}
openPKGinDMG ${CMAKE_DMG}
openPKGinDMG ${SVN_DMG}
openNamedPKGinDMG ${COIN_DMG} "Coin.pkg"
openNamedPKGinDMG ${COIN_DMG} "CoinTools.pkg"
openPKGinDMG ${MISC_DMG}
