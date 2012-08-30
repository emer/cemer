#!/bin/bash
# Loads emergent and libquarter .deb files from your home directory
# into the repositories.  Run as root.
# Usage:
#   ubuntu-repo-update.sh             # Update the latest repo
#   ubuntu-repo-update.sh main        # Update the main repo
#   ubuntu-repo-update.sh main latest # Update the main and latest repos

RUN_ON=grey
if ! grep -q "$RUN_ON" /etc/hostname; then
  echo "ERROR: This script should be run on ${RUN_ON}"
  exit
fi
if [ "`id -u`" != "0" ]; then
  echo "Running script as root..."
  sudo "$@"
  exit
fi

CWD=`pwd`
DIST_ROOT=/usr/local/ubuntu

# By default, only update the 'latest' repo.  Or, if a repo was given on
# the command line (such as 'main'), or multiple repos were given (such as
# 'latest main'), then update that repo(s) instead.
# Generally the main repo is only updated once per semester.
REPOS=latest
if [ -n "$1" ]; then
  REPOS="$1"
fi

for REPO in $REPOS; do
  for DIST in lucid maverick natty oneiric; do
    for ARCH in i386 amd64; do
      echo "Copying ${ARCH} files ..."
      SUBDIR=dists/${DIST}/${REPO}/binary-${ARCH}
      mkdir -p ${DIST_ROOT}/${SUBDIR}
      cd ${DIST_ROOT}
      for PACKAGE in emergent libquarter; do
        FILE=${PACKAGE}*${ARCH}.deb
        FILEPATH=${CWD}/${DIST}/${FILE}
        echo "  Copying ${FILEPATH} to ${SUBDIR} ..."
        cp ${FILEPATH} ${SUBDIR}
        chmod 644 ${SUBDIR}/${FILE}
        chown root.root ${SUBDIR}/${FILE}
      done
      echo "  Updating ${ARCH} repository ..."
      dpkg-scanpackages ${SUBDIR} /dev/null | grep -v -E 'Depends:[^a-z]$' > ${SUBDIR}/Packages
      gzip -f ${SUBDIR}/Packages
    done
  done
done

echo -e "\n\nNOTE: Warnings about 'override file' are expected\n"

echo -e "\n\nUpdated repos: ${REPOS}"
echo "Press enter to delete original .deb files,"
read -p "or Ctrl-C if another repo needs to be updated " JUNK

# Now that all repos have been updated, delete the files.
for DIST in lucid maverick natty oneiric; do
  for ARCH in i386 amd64; do
    for PACKAGE in emergent libquarter; do
      FILE=${PACKAGE}*${ARCH}.deb
      FILEPATH=${CWD}/${DIST}/${FILE}
      echo "  Deleting ${FILEPATH}..."
      rm ${FILEPATH}
    done
  done
done
