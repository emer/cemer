#!/bin/bash
# This script builds debian packages for Emergent.
# It has been tested on Ubuntu 10.10.

ISSUE="Ubuntu 10.10"
if ! grep -q "$ISSUE" /etc/issue; then
  echo "ERROR: This script should be run on ${ISSUE}"
  exit
fi

echo "You may need to provide your password for sudo a few times for this script."

# Make sure the maverick-backports repo is enabled so we can get cmake 2.8.3
# (cmake 2.8.2 has a bug that prevents creating .deb packages)
REPOS=/etc/apt/sources.list
FIND_BACKPORTS="^\s*deb\s\s*http://us.archive.ubuntu.com/ubuntu/\s\s*maverick-backports"
if ! grep -q $FIND_BACKPORTS $REPOS; then
  echo "Need to add the maverick-backports repository to get cmake 2.8.3"
  REPOS_BACKUP="${REPOS}-backup"
  echo "Backing up $REPOS to $REPOS_BACKUP..."
  sudo cp $REPOS $REPOS_BACKUP || exit
  echo "Trying to uncomment maverick-backports repo line..."
  sudo sed -i '/^\s*##*\s*deb\(-src\)\{0,1\}\s\s*http:\/\/us.archive.ubuntu.com\/ubuntu\/\s\s*maverick-backports/s/^\s*##*\s*//' $REPOS
  if ! grep -q $FIND_BACKPORTS $REPOS; then
    echo "Didn't find a commented out maverick-backports line; creating new"
    sudo sh -c "echo 'deb http://us.archive.ubuntu.com/ubuntu/ maverick-backports main restricted universe multiverse' >> $REPOS"
  fi
fi

# Double check that maverick-backports is enabled.
if grep -q $FIND_BACKPORTS $REPOS; then
  echo "Found maverick-backport repository"
else
  echo "Could not add maverick-backport repository needed for cmake 2.8.3.  Quitting."
  exit
fi

# Install prereq packages
# Need to update to make sure we see the backport repo and get the latest cmake.
sudo apt-get -qq update
# This list should match the CPACK_DEBIAN_PACKAGE_DEPENDS line of emergent/CMakeModules/EmergentCPack.cmake
# Except:
#  * only need checkinstall here to make the Quarter package.
#  * don't need libquarter here since we will be building it ourselves.
sudo apt-get -y install checkinstall subversion cmake g++ libqt4-dev libcoin60-dev libreadline6-dev libgsl0-dev zlib1g-dev libode-sp-dev libpng-dev libjpeg8-dev
# Remove any existing libquarter and emergent installations
sudo apt-get -y remove emergent libquarter

NUM_PROCS=`grep "^processor\s*:\s*[0-9]*$" /proc/cpuinfo |wc -l`

# Download, compile, install, and package Quarter
TMPDIR=$HOME/debbuild
mkdir -p $TMPDIR
cd $TMPDIR
wget ftp://grey.colorado.edu/pub/emergent/Quarter-latest.tar.gz
# Original source was: http://ftp.coin3d.org/coin/src/all/Quarter-1.0.0.tar.gz
tar zxf Quarter-latest.tar.gz
cd Quarter
./configure
# Emergent wiki notes said to get rid of the plugins subdirectory, no reason given.
sed -i '/^SUBDIRS *=/s/ *plugins */ /' src/Makefile
# checkinstall's version of mkdir doesn't implement -p correctly, so work around it.
sed -i 's/if mkdir .*-p --.*; then/if false; then ## &/' cfg/mkinstalldirs
make -j $NUM_PROCS
# If --install=no, it's possible to make a .deb without sudo, but we need quarter to be installed to compile Emergent so may as well.
sudo checkinstall -D -y --install=yes --pkgname=libquarter --pkgversion=1.0.0 --arch=i386 --pkglicense=GPL --maintainer=emergent-users@grey.colorado.edu --reset-uids=yes

cd $TMPDIR
svn checkout --username anonymous --password emergent http://grey.colorado.edu/svn/emergent/emergent/trunk emergent/
cd emergent
mkdir -p build
./configure
cd build
make -j $NUM_PROCS package

cd $TMPDIR
echo -e "\n\n\n"
echo "If all went well, copy the libquarter*.deb and emergent*.deb files"
echo "to your home directory on grey and run update-ubuntu-repo.sh"
echo ""
echo "scp $TMPDIR/Quarter/libquarter*.deb $TMPDIR/emergent/build/emergent*.deb dpfurlani@grey.colorado.edu:/home/dpfurlani/"
echo "ssh dpfurlani@grey.colorado.edu"
echo "sudo update-ubuntu-repo.sh"
echo ""

