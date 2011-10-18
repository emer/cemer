#!/bin/bash
# This script builds debian packages for emergent.
# It has been tested on Ubuntu 10.04, 10.10, and 11.04.
set -e

# Make sure we're on the right kind of Linux.
# (could use /usr/bin/lsb_release to get this info)
NEED_BACKPORT="n"
ISSUE="Ubuntu 11.10"
if ! grep -q "$ISSUE" /etc/issue; then
  ISSUE="Ubuntu 11.04"
  if ! grep -q "$ISSUE" /etc/issue; then
    # Older releases need the backports repo for cmake.
    NEED_BACKPORT="y"
    ISSUE="Ubuntu 10.10"
    if ! grep -q "$ISSUE" /etc/issue; then
      ISSUE="Ubuntu 10.04"
      if ! grep -q "$ISSUE" /etc/issue; then
        echo "ERROR: This script should be run on ${ISSUE} or higher"
        exit
      fi
    fi
  fi
fi
REPONAME=`lsb_release -cs`

echo "Note: you must provide your password for sudo a few times for this script."

# Get emergent svn revision to build
REV="$1"
if [ -z $REV ]; then
  read -p "Enter the emergent svn revision number to retrieve: [HEAD] " REV
  if [ -z $REV ]; then REV="HEAD"; fi
fi

TAG="$2"
if [ -z $TAG ]; then
  read -p "Enter the emergent tag to use: [trunk] " TAG
  if [ -z $TAG ]; then TAG="trunk"; fi
fi
if [ $TAG != "trunk" ]; then TAG="tags/$TAG"; fi

# Check if the Quarter library should be rebuilt.  Defaults to yes, unless
# it is already installed.
BUILD_QUARTER="y"
if dpkg-query -s libquarter0 2>&1 | grep -q "Status.*installed"; then
  BUILD_QUARTER="n"
  read -p "Rebuild the Quarter library from source? [y/N]: " BUILD_QUARTER
  if [ "$BUILD_QUARTER" == "Y" ]; then BUILD_QUARTER="y"; fi
fi

# Make sure the backports repo is enabled so we can get cmake 2.8.3 on
# maverick (cmake 2.8.2 has a bug that prevents creating .deb packages
# that work) and cmake 2.8.1 on lucid (cmake 2.8.0 has some other bug).
if [ "$NEED_BACKPORT" == "y" ]; then
  REPOS=/etc/apt/sources.list
  FIND_BACKPORTS="^\s*deb\s\s*http://us.archive.ubuntu.com/ubuntu/\s\s*${REPONAME}-backports"
  if ! grep -q $FIND_BACKPORTS $REPOS; then
    echo "Need to add the ${REPONAME}-backports repository to get good cmake version"
    REPOS_BACKUP="${REPOS}-backup"
    echo "Backing up $REPOS to $REPOS_BACKUP..."
    sudo cp $REPOS $REPOS_BACKUP
    echo "Trying to uncomment ${REPONAME}-backports repo line..."
    sudo sed -i "/^\s*##*\s*deb\(-src\)\{0,1\}\s\s*http:\/\/us.archive.ubuntu.com\/ubuntu\/\s\s*${REPONAME}-backports/s/^\s*##*\s*//" $REPOS
    if ! grep -q $FIND_BACKPORTS $REPOS; then
      echo "Didn't find a commented out ${REPONAME}-backports line; creating new"
      sudo sh -c "echo 'deb http://us.archive.ubuntu.com/ubuntu/ ${REPONAME}-backports main restricted universe multiverse' >> $REPOS"
    fi
  fi
  
  # Double check that backports is enabled.
  if grep -q $FIND_BACKPORTS $REPOS; then
    echo "Found ${REPONAME}-backport repository"
  else
    echo "Could not add ${REPONAME}-backport repository needed for cmake upgrade.  Quitting."
    exit
  fi
fi

# Install prereq packages
# Need to update to make sure we see the backport repo and get the latest cmake.
echo "Updating repositories..."
sudo apt-get -qq update | sed 's/^/  /'

# Packages needed for debuild
DEBUILD_PKGS="build-essential gnupg lintian fakeroot debhelper dh-make subversion-tools devscripts mercurial"

# Packages needed to build emergent
# This list should match the CPACK_DEBIAN_PACKAGE_DEPENDS line of emergent/CMakeModules/EmergentCPack.cmake
# Except:
#  * only need checkinstall here to make the Quarter package.
#    * (that was the old way of packaging)
#  * don't need libquarter here since we will be building it ourselves.
EMERGENT_PKGS="checkinstall subversion cmake g++ libqt4-dev libcoin60-dev libreadline6-dev libgsl0-dev zlib1g-dev libode-sp-dev libpng-dev libjpeg-dev"

# Packages needed to build Quarter.  It also requires some of the
# ones already listed in EMERGENT_PKGS, but this one is specifically
# needed for building Quarter (otherwise compile error).
QUARTER_PKGS="pkg-config"

# Use a separate xterm window to track progress
XTERM=`which xterm`

echo -e "\nInstalling packages needed to build (log will open in separate xterm)..."
echo "  (ctrl-c in *this* window will kill the package install process)"
OUTPUT="apt-get-install-output.txt"
test -x "$XTERM" && $XTERM -si -geometry 160x50 -T "apt-get install progress (safe to close this window)" -e tail -F $OUTPUT &
sudo apt-get -q -y install $DEBUILD_PKGS $EMERGENT_PKGS $QUARTER_PKGS 2>&1 > $OUTPUT

# This may fail (expectedly) if the packages aren't already installed,
# so allow it with the echo alternative (otherwise set -e would bail).
if [ "$BUILD_QUARTER" == "y" ]; then
  echo "Removing any existing libquarter and emergent installations..."
  sudo apt-get -q -y remove emergent libquarter libquarter0 | sed 's/^/  /' || echo "(OK)"
else
  echo "Removing any existing emergent installations..."
  sudo apt-get -q -y remove emergent libquarter | sed 's/^/  /' || echo "(OK)"
fi

# Make sure we don't keep an old copy of libquarter0 in the cache,
# since later we need to install the one we built.
sudo apt-get -qq autoremove | sed 's/^/  /'

# If we're not already in the build scripts directory,
# then get it and change into it
if [ ! -x ./ubuntu-motu-emergent ]; then
  svn checkout http://grey.colorado.edu/svn/emergent/emergent/trunk/package
  cd package
fi

if [ "$BUILD_QUARTER" == "y" ]; then
  echo -e "\nBuilding and packaging Quarter (log will open in separate xterm)..."
  echo "  (ctrl-c in *this* window will kill the build/package process)"
  OUTPUT="libQuarter-build-output.txt"
  test -x "$XTERM" && $XTERM -si -geometry 160x50 -T "libQuarter build progress (safe to close this window)" -e tail -F $OUTPUT &
  ./ubuntu-motu-quarter 2>&1 > $OUTPUT

  echo -e "\nInstalling the Quarter libraries before building emergent..."
  sudo dpkg -i /tmp/libquarter0_*.deb
fi

echo -e "\nBuilding and packaging emergent (log will open in separate xterm)..."
echo "  (ctrl-c in *this* window will kill the build/package process)"
OUTPUT="emergent-build-output.txt"
test -x "$XTERM" && $XTERM -si -geometry 160x50 -T "emergent build progress (safe to close this window)" -e tail -F $OUTPUT &
./ubuntu-motu-emergent $REV $TAG 2>&1 > $OUTPUT

DEBS="/tmp/emergent*.deb"
if [ "$BUILD_QUARTER" == "y" ]; then
  DEBS="/tmp/libquarter*.deb ${DEBS}"
fi

cat <<INSTRUCTIONS

Done!


** If all went well, copy the libquarter*.deb and emergent*.deb files
** to your home directory on grey and run update-ubuntu-repo.sh
**
** scp ${DEBS} dpfurlani@grey.colorado.edu:/home/dpfurlani/$REPONAME/
** ssh dpfurlani@grey.colorado.edu
** sudo update-ubuntu-repo.sh

INSTRUCTIONS

