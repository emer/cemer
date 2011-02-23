#!/bin/bash
# This script builds debian packages for Emergent.
# It has been tested on Ubuntu 10.04 and 10.10.
set -e

# Make sure we're on the right kind of Linux.
# (could use /usr/bin/lsb_release to get this info)
ISSUE="Ubuntu 10.10"
if ! grep -q "$ISSUE" /etc/issue; then
  ISSUE="Ubuntu 10.04"
  if ! grep -q "$ISSUE" /etc/issue; then
    echo "ERROR: This script should be run on ${ISSUE} or higher"
    exit
  fi
fi
REPONAME=`lsb_release -cs`

echo "Note: you may need to provide your password for sudo a few times for this script."

# Get Emergent svn revision to build
REV="$1"
if [ -z $REV ]; then
  read -p "Enter the Emergent svn revision number to retrieve: [HEAD] " REV
  if [ -z $REV ]; then REV="HEAD"; fi
fi

ARCH=`dpkg-architecture -qDEB_BUILD_ARCH`

# Make sure the backports repo is enabled so we can get cmake 2.8.3 on
# maverick (cmake 2.8.2 has a bug that prevents creating .deb packages
# that work) and cmake 2.8.1 on lucid (cmake 2.8.0 has some other bug).
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

# Install prereq packages
# Need to update to make sure we see the backport repo and get the latest cmake.
echo "Updating repositories..."
sudo apt-get -qq update

# Packages needed for debuild
DEBUILD_PKGS="build-essential gnupg lintian fakeroot debhelper dh-make subversion-tools devscripts mercurial"

# Packages needed to build Emergent
# This list should match the CPACK_DEBIAN_PACKAGE_DEPENDS line of emergent/CMakeModules/EmergentCPack.cmake
# Except:
#  * only need checkinstall here to make the Quarter package.
#    * (that was the old way of packaging)
#  * don't need libquarter here since we will be building it ourselves.
EMERGENT_PKGS="checkinstall subversion cmake g++ libqt4-dev libcoin60-dev libreadline6-dev libgsl0-dev zlib1g-dev libode-sp-dev libpng-dev libjpeg-dev"

echo "Installing packages needed to build..."
sudo apt-get -q -y install $DEBUILD_PKGS $EMERGENT_PKGS | sed 's/^/  /'

echo "Removing any existing libquarter and emergent installations..."
sudo apt-get -q -y remove emergent libquarter libquarter0 | sed 's/^/  /' || echo "(OK)"

# If we're not already in the build scripts directory,
# then get it and change into it
if [ ! -x ./ubuntu-motu-quarter ]; then
  svn checkout http://grey.colorado.edu/svn/emergent/emergent/trunk/package
  cd package
fi

# Use a separate xterm window to track progress
XTERM=`which xterm`

echo "Building and packaging Quarter (log will open in separate xterm)..."
echo "  (ctrl-c in *this* window will kill the build/package process)"
OUTPUT=libQuarter-build-output.txt
test -x $XTERM && $XTERM -T "libQuarter build progress (safe to close this window)" -e tail -F $OUTPUT &
./ubuntu-motu-quarter 2>&1 > $OUTPUT

echo "Installing the Quarter libraries before building Emergent..."
sudo dpkg -i /tmp/libquarter0_*.deb

echo "Building and packaging Emergent (log will open in separate xterm)..."
echo "  (ctrl-c in *this* window will kill the build/package process)"
OUTPUT=emergent-build-output.txt
test -x $XTERM && $XTERM -T "Emergent build progress (safe to close this window)" -e tail -F $OUTPUT &
./ubuntu-motu-emergent $REV 2>&1 > $OUTPUT

cat <<INSTRUCTIONS

Done!


** If all went well, copy the libquarter*.deb and emergent*.deb files
** to your home directory on grey and run update-ubuntu-repo.sh
**
** scp /tmp/libquarter*.deb /tmp/emergent*.deb dpfurlani@grey.colorado.edu:/home/dpfurlani/$REPONAME/
** ssh dpfurlani@grey.colorado.edu
** sudo update-ubuntu-repo.sh

INSTRUCTIONS

