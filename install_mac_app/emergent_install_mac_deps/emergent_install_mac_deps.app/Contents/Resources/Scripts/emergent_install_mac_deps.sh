#!/bin/bash

# install emergent mac dependencies script
# see http://grey.colorado.edu/emergent

# As of emergent version 8.5, we are now using homebrew versions of all the
# dependencies except qt and coin

# this should be the only thing you need to update: grab the latest versions
QT_DMG=qt5931_mac64.dmg
COIN_DMG=coin_mac64_4_0.dmg


OS_VERS=`sw_vers | grep ProductVersion | cut -f2 | cut -f2 -d.`
echo "installing on OSX version: $OS_VERS"

if (( $OS_VERS < 9 )); then
    echo "SORRY: version $OS_VERS of Mac OSX is not supported for this version of the software"
    exit 1
fi

#FTP_REPO=ftp://grey.colorado.edu/pub/emergent
#FTP_CMD="/usr/bin/ftp -ai"

FTP_REPO=https://grey.colorado.edu/emergent_ftp
FTP_CMD="/usr/bin/curl --progress-bar -O"

DOWNLOAD_DIR=$HOME/Downloads
 
if [[ ! -d $DOWNLOAD_DIR ]]; then
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

downloadFTP ${COIN_DMG}
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

function chownUserDir {
    # Argument $1 is directory to chown
    if [[ -d "$1" ]]; then
	echo "directory: $1 exists -- doing sudo chown -R to user to allow brew to overwrite"
	sudo chown -R $USER "$1"
    fi
}
    
    
function installHomeBrew {
    echo " "
    echo "================================================="
    echo "        Installing HomeBrew itself"
    echo "================================================="
    echo " "
    echo "Installing homebrew -- commands will echo below:"
    echo " "
    echo "IMPORTANT: if any of these commands fail, you may already have this installed, or may need to intervene manually.."
    echo " "
    set -v  # turn on verbose
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    set +v
    echo " "
    echo "Some /usr/local subdirectories need to be owned by user for brew link to work"
    set -v  # turn on verbose
    chownUserDir /usr/local/share/doc
    chownUserDir /usr/local/share/emacs
    chownUserDir /usr/local/include/gsl
    chownUserDir /usr/local/include/ode
    set +v
    echo " "
    echo "-- done with commands: again if any of these commands fail, you may need to intervene manually.."
    echo " "
}

function installBrewDeps {
    echo " "
    echo "================================================="
    echo "        Installing dependencies using HomeBrew"
    echo "================================================="
    echo " "
    echo "IMPORTANT: if any of these commands fail, you may already have this installed, or may need to intervene manually.."
    echo " "
    set -v  # turn on verbose
    
    /usr/local/bin/brew install --force pkg-config
    /usr/local/bin/brew install --force cmake
    /usr/local/bin/brew install --force apr
    /usr/local/bin/brew install --force apr-util
    /usr/local/bin/brew install --force subversion
    /usr/local/bin/brew install --force gsl ode libjpeg libpng
    /usr/local/bin/brew install --force libsndfile
    
    /usr/local/bin/brew upgrade pkg-config
    /usr/local/bin/brew upgrade cmake
    /usr/local/bin/brew upgrade apr
    /usr/local/bin/brew upgrade apr-util
    /usr/local/bin/brew upgrade subversion
    /usr/local/bin/brew upgrade gsl ode libjpeg libpng
    /usr/local/bin/brew upgrade libsndfile

    /usr/local/bin/brew link --force --overwrite pkg-config
    /usr/local/bin/brew link --force --overwrite cmake
    /usr/local/bin/brew link --force --overwrite apr
    /usr/local/bin/brew link --force --overwrite apr-util
    /usr/local/bin/brew link --force --overwrite subversion
    /usr/local/bin/brew link --force --overwrite gsl ode libjpeg libpng
    /usr/local/bin/brew link --force --overwrite libsndfile

    set +v
    echo " "
    echo "-- done with commands: again if any of these commands fail, you may need to intervene manually.."
    echo " "
}

echo " "
echo "================================================="
echo "        Step 2: Installing The Packages"
echo "================================================="
echo " "

# there are no dependencies anymore

installPKGinDMG ${QT_DMG}
installPKGinDMG ${COIN_DMG}

installHomeBrew
installBrewDeps

echo " "
echo "================================================="
echo "        INSTALLATION WAS SUCCESSFUL!!"
echo "================================================="
echo " "
