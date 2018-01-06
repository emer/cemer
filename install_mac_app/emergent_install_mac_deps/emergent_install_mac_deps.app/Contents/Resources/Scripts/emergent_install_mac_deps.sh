#!/bin/bash

# install emergent mac dependencies script
# see http://grey.colorado.edu/emergent

# this should be the only thing you need to update: grab the latest versions
QT_DMG=qt561_mac64.dmg
COIN_DMG=coin_mac64_4_0.dmg
CMAKE_DMG=cmake-3.6.1_mac64.dmg
MISC_DMG=misclibs_mac64_8_0.dmg
# using homebrew for svn entirely
NEEDS_BREW_SVN="true"

# these are now obsolete
#QUARTER_DMG=quarter_mac64_qt56.dmg
#SVN_DMG=Subversion-1.9.4_10.11.x.dmg


OS_VERS=`sw_vers | grep ProductVersion | cut -f2 | cut -f1,2 -d.`
echo "installing on OSX version: $OS_VERS"

if [[ "$OS_VERS" == "10.7" ]]; then
    echo "SORRY: version 10.7 of Mac OSX is not supported for this version of the software"
    exit 1
fi

if [[ "$OS_VERS" == "10.8" ]]; then
    echo "SORRY: version 10.8 of Mac OSX is not supported for this version of the software"
    exit 1
fi

if [[ "$OS_VERS" == "10.9" ]]; then
    echo "SORRY: version 10.9 of Mac OSX is not supported for this version of the software"
    exit 1
fi

# if [[ "$OS_VERS" == "10.10" ]]; then
#     echo "Note: updating the dependencies for 10.10"
#     SVN_DMG=Subversion-1.9.4_10.10.x.dmg
# fi

# if [[ "$OS_VERS" == "10.12" ]]; then
#     echo "Note: updating the dependencies for 10.12: requires homebrew apr for Sierra"
#     NEEDS_BREW_APR=true
# fi

#FTP_REPO=ftp://grey.colorado.edu/pub/emergent
#FTP_CMD="/usr/bin/ftp -ai"

FTP_REPO=https://grey.colorado.edu/emergent_ftp
FTP_CMD="/usr/bin/curl --progress-bar -O"

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
downloadFTP ${COIN_DMG}
downloadFTP ${CMAKE_DMG}
downloadFTP ${QT_DMG}

#downloadFTP ${QUARTER_DMG}
#downloadFTP ${SVN_DMG}

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
  sudo /bin/rm /usr/local/bin/ccmake /usr/local/bin/cmake /usr/local/bin/cmake-gui /usr/local/bin/cmakexbuild /usr/local/bin/cpack /usr/local/bin/ctest
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
  sudo /usr/sbin/installer -target / -pkg "${DMG_PKG}"
  unmountDMG $DMG_MNT
  # make new links:
  sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install 
  echo "===> NOTE: if you see errors above here about links already existing"
  echo "then you may need to manually create cmake etc links in /usr/local/bin/"
}

function installHomeBrewSVN {
    echo " "
    echo "================================================="
    echo "        Step 3: Installing apr, -util, and svn"
    echo "================================================="
    echo " "
    echo "Installing apache apr, apr-util and subversion from homebrew, doing the following commands:"
    echo '/usr/bin/ruby -e \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)\"'
    echo "brew install --force apr"
    echo "brew install --force apr-util"
    echo "brew install --force subversion"
    echo "brew link --force --overwrite apr"
    echo "brew link --force --overwrite apr-util"
    echo "brew link --force --overwrite subversion"
    echo " "
    echo "IMPORTANT: if any of these commands fail, you may already have this installed, or may need to intervene manually.."
    echo " "
    
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    brew install --force apr
    brew install --force apr-util
    brew install --force subversion
    brew link --force --overwrite apr
    brew link --force --overwrite apr-util
    brew link --force --overwrite subversion

    #
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
installCMAKEinDMG ${CMAKE_DMG}
installPKGinDMG ${QT_DMG}
installPKGinDMG ${COIN_DMG}

#installPKGinDMG ${QUARTER_DMG}
#installPKGinDMG ${SVN_DMG}

if [[ "$NEEDS_BREW_SVN" == "true" ]]; then
    installHomeBrewSVN
fi    

echo " "
echo "================================================="
echo "        INSTALLATION WAS SUCCESSFUL!!"
echo "================================================="
echo " "
