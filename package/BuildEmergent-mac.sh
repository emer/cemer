#!/bin/bash
# This script builds a Mac package of emergent.
set -e

# Make sure this is a mac.
if ! uname -a | grep -q Darwin; then
  echo "ERROR: This script should be run on a Mac"
  exit
fi

# Get emergent svn revision to build.
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

ARCH="$3"
if [ -z $ARCH ]; then
  read -p "Build 32-bit or 64-bit installer? [64] " ARCH
  if [ -z $ARCH ]; then ARCH="64"; fi
fi

# Update source code and make package.
EMERGENT_SRC_DIR=~/emergent
mkdir -p ${EMERGENT_SRC_DIR}
cd ${EMERGENT_SRC_DIR}
svn checkout -r ${REV} http://grey.colorado.edu/svn/emergent/emergent/${TAG} .
./configure --emer-mac-arch-bits=${ARCH}
cd build
NCPU=`sysctl -n hw.ncpu`
make -j ${NCPU} package

# Do this after building since config.h is a generated file.
EMERGENT_VERSION=`sed -n '/VERSION/s/.*"\(.*\)".*/\1/p' < ${EMERGENT_SRC_DIR}/config.h`
echo -e "\nBuilt emergent version ${EMERGENT_VERSION}-${REV}\n"

# The package just build by 'make package' is an upgrader, not a full installer.
# Move it to the home directory and put the svn rev and 'upgrade' in its name.
INSTALL_DMG=~/emergent-${EMERGENT_VERSION}-${REV}-mac${ARCH}.dmg
UPGRADE_DMG=~/emergent-${EMERGENT_VERSION}-${REV}-upgrade-mac${ARCH}.dmg
echo "Moving upgrader package to home directory ..."
mv -f emergent-${EMERGENT_VERSION}-mac.dmg ${UPGRADE_DMG}

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

# Find the previous full-dmg directory and rename it for the current metapackage.
# This is bootstrapping from the previous full-installer (see TODO below).
# Note: these directories contain spaces, so always put them in quotes.
cd ~
OLD_DMG_DIR="`ls -1rtd Emergent\ *.*.* | tail -1`"
# If no old DMG dir found, make it.
if [ -z "${OLD_DMG_DIR}" ]; then
  echo "Could not find existing package directory matching 'Emergent\ *.*.*'"
  echo "Will attempt to create from emergent*.dmg file..."
  OLD_DMG="`ls -1rtd emergent-*.*.*-*[0-9]-mac${ARCH}.dmg | tail -1`"
  if [ -z "${OLD_DMG}" ]; then
    echo "Could not find existing package matching 'emergent-*.*.*-*[0-9]-mac${ARCH}.dmg'."
    echo "Please download the latest full Mac package to your home directory,"
    echo "then re-run this script."
    exit
  fi

  echo "Mounting package ${OLD_DMG}..."
  OLD_DMG_MNT="`mountDMG ${OLD_DMG}`"
  OLD_DMG_DIR=~/`basename "${OLD_DMG_MNT}"`
  echo "Copying files from '${OLD_DMG_MNT}' to '${OLD_DMG_DIR}'"
  mkdir "${OLD_DMG_DIR}"
  cp -a "${OLD_DMG_MNT}/Emergent.mpkg" "${OLD_DMG_DIR}/Emergent.mpkg"

  echo "Unmounting ${OLD_DMG_MNT}"
  unmountDMG "${OLD_DMG_MNT}"
fi

DMG_DIR=~/"Emergent ${EMERGENT_VERSION}"
if [ -d "${DMG_DIR}" ]; then
  echo "Directory already exists: ${DMG_DIR}"
  # exit
else
  echo "Renaming ${OLD_DMG_DIR} --> ${DMG_DIR} ..."
  mv "${OLD_DMG_DIR}" "${DMG_DIR}"
fi

# Get rid of the previous (existing) 'upgrader' subpackage.
echo "Removing old subpackage ..."
cd "${DMG_DIR}/Emergent.mpkg/Contents/"
rm -rf Packages/emergent-5.*.*-mac.pkg

# Mount the upgrader and extract its subpackage.
echo "Mounting package ${UPGRADE_DMG} ..."
UPGRADE_MNT=`mountDMG ${UPGRADE_DMG}`
UPGRADE_PKG=emergent-${EMERGENT_VERSION}-mac.pkg
# No slash after src dir -- want to copy the whole directory, not just its contents.
echo "Copying from: ${UPGRADE_MNT}/${UPGRADE_PKG} to: Packages/"
cp -a ${UPGRADE_MNT}/${UPGRADE_PKG} Packages/
# Don't need the upgrader anymore.
echo "Unmounting ${UPGRADE_MNT} ..."
unmountDMG ${UPGRADE_MNT}

# Get the size of the new emergent package.
PKG_SIZE=`sed -n '/IFPkgFlagInstalledSize/{n;s/.*>\([0-9]*\)<.*/\1/p;}' < Packages/${UPGRADE_PKG}/Contents/Info.plist`
echo "Size of new emergent package is ${PKG_SIZE}."

# Update the distribution file with the new version number and subpackage size.
DIST_FILE="distribution.dist"
BKUP=".bkup"
echo "Updating ${DIST_FILE} file ..."
sed -i "${BKUP}" "
  s/Emergent version 5\.[0-9][0-9]*\.[0-9][0-9]*/Emergent version ${EMERGENT_VERSION}/
  s/emergent-5\.[0-9][0-9]*\.[0-9][0-9]*/emergent-${EMERGENT_VERSION}/
  /emergent-${EMERGENT_VERSION}-mac.pkg/s/installKBytes=\"[0-9]*\"/installKBytes=\"${PKG_SIZE}\"/
" ${DIST_FILE}
rm ${DIST_FILE}${BKUP}

cd ~
echo "Creating metapackage.  Be patient, this will take a while."
if [ -f "${INSTALL_DMG}" ]; then
  echo "File already exists, will be deleted first: ${INSTALL_DMG}"
  rm -rf "${INSTALL_DMG}"
fi
hdiutil create -srcdir "${DMG_DIR}" "${INSTALL_DMG}"

DMGS="${INSTALL_DMG} ${UPGRADE_DMG}"
cat <<INSTRUCTIONS

Done!


** If all went well, copy the two .dmg files to your home
** directory on grey and run update-win-mac.sh
**
** scp ${DMGS} dpfurlani@grey.colorado.edu:/home/dpfurlani/
** ssh dpfurlani@grey.colorado.edu
** sudo update-win-mac.sh
**
** Also, make an SVN tag:
**
** svn copy -r ${REV} http://grey.colorado.edu/svn/emergent/emergent/trunk http://grey.colorado.edu/svn/emergent/emergent/tags/${EMERGENT_VERSION} -m "emergent ${EMERGENT_VERSION} (beta) was built from svn revision ${REV}"

INSTRUCTIONS

# Only want to do this once per release.
if [ ${ARCH} == "64" ]; then
  svn copy -r ${REV} http://grey.colorado.edu/svn/emergent/emergent/trunk http://grey.colorado.edu/svn/emergent/emergent/tags/${EMERGENT_VERSION} -m "emergent ${EMERGENT_VERSION} (beta) was built from svn revision ${REV}"
fi

scp ${DMGS} dpfurlani@grey.colorado.edu:/home/dpfurlani/

exit


# # Instructions to upgrade the Qt libraries (or any other prereq package).
#
# # Mount the new Qt installer and copy its contents into the emergent
# # full installation package directory.
# cd ~/Emergent\ 5.1.2/..../?
# rm -rf Qt_*
# cp -a /Volumes/Qt\ 4.7.1/packages/Qt_*.pkg .
# 
# # get package reference lines for the Qt packages
# cd ~/emergent-5.1.2-mac64/emergent-5.1.2-mac64.mpkg/Contents
# for pkg in docs examples headers imports libs plugins tools translations; do
#   pkg2=$pkg
#   if [ "$pkg2" == "libs" ]; then
#     pkg2="libraries"
#   fi
#   file="Packages/Qt_${pkg2}.pkg"
#   file2="$file/Contents/Info.plist"
#   size=`sed -n '/IFPkgFlagInstalledSize/{n;s/.*>\([0-9]*\)<.*/\1/p;}' < $file2`
#   echo "    <pkg-ref id=\"com.trolltech.qt4.${pkg}\" installKBytes=\"${size}\" version=\"4.7\" auth=\"Root\">file:./Contents/${file}</pkg-ref>"
# done
#
# manually edit the new Qt lines into the distribution.dist file
#   also manually edit in the new Qt_imports.pkg
#   also change 4.6.2 to 4.7.1

