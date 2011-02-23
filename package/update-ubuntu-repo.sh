#!/bin/bash
# Loads emergent and libquarter .deb files from your home directory
# into the repositories.  Run as root.
RUN_ON=grey
if ! grep -q "$RUN_ON" /etc/hostname; then
  echo "ERROR: This script should be run on ${RUN_ON}"
  exit
fi
if [ "`id -u`" != "0" ]; then
  echo "ERROR: This script should be run as root.  Try this:"
  echo "  sudo $0"
  exit
fi

CWD=`pwd`
DIST_ROOT=/usr/local/ubuntu
# Don't mess with the 'main' repositories, those should be stable
# for months at a time.  Make a new repo called "latest".
REPO=latest
for DIST in lucid maverick; do
  for ARCH in i386 amd64; do
    echo "Moving ${ARCH} files ..."
    SUBDIR=dists/${DIST}/${REPO}/binary-${ARCH}
    mkdir -p ${DIST_ROOT}/${SUBDIR}
    cd ${DIST_ROOT}
    for PACKAGE in emergent libquarter; do
      FILE=${CWD}/${DIST}/${PACKAGE}*${ARCH}.deb
      echo "  Moving ${FILE} to ${SUBDIR} ..."
      mv ${FILE} ${SUBDIR}
    done
    echo "  Updating ${ARCH} repository ..."
    dpkg-scanpackages ${SUBDIR} /dev/null | grep -v -E 'Depends:[^a-z]$' > ${SUBDIR}/Packages
    gzip -f ${SUBDIR}/Packages
  done
done

echo -e "\n\nNOTE: Warnings about 'override file' are expected\n"
