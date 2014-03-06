#!/bin/bash

# install emergent mac dependencies script
# see http://grey.colorado.edu/emergent

# this should be the only thing you need to update: grab the latest versions
QT_DMG=qt52_mac64.dmg
COIN_QUARTER_DMG=coin_quarter_mac64_qt52.dmg
CMAKE_DMG=cmake-2.8.12.2-Darwin64-universal.dmg
SVN_DMG=Subversion-1.8.8_10.9.x.dmg
MISC_DMG=emergent_misc_deps_mac64.dmg

OS_VERS=`sw_vers | grep ProductVersion | cut -f2 | cut -f1,2 -d.`
echo "installing on OSX version: $OS_VERS"

if [[ "$OS_VERS" == "10.7" ]]; then
    echo "SORRY: version 10.7 of Mac OSX is not supported for this version of the software"
    exit 1
fi

if [[ "$OS_VERS" == "10.8" ]]; then
    echo "Note: updating the dependencies for 10.8"
    COIN_QUARTER_DMG=coin_quarter_mac64_qt52_10_8.dmg
    SVN_DMG=Subversion-1.7.8_10.8.x.dmg
fi
 
FTP_REPO=ftp://grey.colorado.edu/pub/emergent
FTP_CMD="/usr/bin/ftp -ai"

DOWNLOAD_DIR=$HOME/Downloads
 
if [[ ! -d $DOWNLOAD_DIR ]]
then
  mkdir $DOWNLOAD_DIR
fi

cd $DOWNLOAD_DIR

function downloadFTP {
  # ARG is file to download - goes to download dir, checks for existence already
  if [ ! -e "$1" ]; then
      echo "downloading file: $1 to $DOWNLOAD_DIR"
      ${FTP_CMD} "${FTP_REPO}/$1"
  else
      echo "file $1 is already downloaded"
  fi
}

echo " "
echo "================================================="
echo "        Step 1: Downloading The Packages"
echo "================================================="
echo " "

downloadFTP ${MISC_DMG}
downloadFTP ${COIN_QUARTER_DMG}
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

function openAPPinDMG {
  # Argument should be the name of the DMG.
  # finds the package file, opens it

  DMG_MNT="`mountDMG $1`"
  echo "mounted: $DMG_MNT"
  DMG_PKG="`ls -1rtd \"$DMG_MNT\"/*app | tail -1`"
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

function installPKGinDMG {
  # Argument $1 is name of the DMG
  # finds the package file, installs it
  echo " "
  echo "================================================="
  echo " => installing package $1"
  echo "================================================="
  echo " "
  DMG_MNT="`mountDMG $1`"
  echo "mounted: $DMG_MNT"
  DMG_PKG="`ls -1rtd \"$DMG_MNT\"/*pkg | tail -1`"
  echo "package: $DMG_PKG"
  echo "IMPORTANT: You may now need to (re)enter your password for sudo here:"
  sudo /usr/sbin/installer -target / -pkg "${DMG_PKG}"
  unmountDMG $DMG_MNT
}

function removeCMakeLinks {
  # This is a major annoying bug in the cmake installer
  # http://public.kitware.com/Bug/view.php?id=10056
  sudo /bin/rm /usr/bin/ccmake /usr/bin/cmake /usr/bin/cmake-gui /usr/bin/cmakexbuild /usr/bin/cpack /usr/bin/ctest
}

function installCMAKEinDMG {
  # Argument $1 is name of the DMG
  # finds the package file, installs it
  echo " "
  echo "================================================="
  echo " => installing package $1"
  echo "================================================="
  echo " "
  DMG_MNT="`mountDMG $1`"
  echo "mounted: $DMG_MNT"
  DMG_PKG="`ls -1rtd \"$DMG_MNT\"/*pkg | tail -1`"
  echo "package: $DMG_PKG"
  echo "IMPORTANT: You may now need to (re)enter your password for sudo here:"
  removeCMakeLinks
  echo "===> NOTE: installer will popup a dialog, usually behind the terminal (it will be jumping up and down in your dock), about installing the command-line links -- please click the install button"
  echo "**THE INSTALL PROCESS WILL STALL UNTIL YOU DO THE AVOVE!!**"
  sudo /usr/sbin/installer -target / -pkg "${DMG_PKG}"
  unmountDMG $DMG_MNT
}

echo " "
echo "================================================="
echo "        Step 2: Installing The Packages"
echo "================================================="
echo " "

# these are listed in order of size and dependency, independent to dependent
# QUARTER depends on COIN, QT
# everything else is independent

installPKGinDMG ${MISC_DMG}
installPKGinDMG ${SVN_DMG}
installCMAKEinDMG ${CMAKE_DMG}
installPKGinDMG ${QT_DMG}
installPKGinDMG ${COIN_QUARTER_DMG}

echo " "
echo "================================================="
echo "        INSTALLATION WAS SUCCESSFUL!!"
echo "================================================="
echo " "
