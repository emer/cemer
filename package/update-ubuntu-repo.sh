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

DIST_ROOT=/usr/local/ubuntu
DIST=maverick
CWD=`pwd`
for ARCH in i386 amd64; do
  echo "Moving ${ARCH} files..."
  SUBDIR=dists/${DIST}/main/binary-${ARCH}
  mkdir -p ${DIST_ROOT}/${SUBDIR}
  cd ${DIST_ROOT}
  for PACKAGE in emergent libquarter; do
    FILE=${CWD}/${PACKAGE}*${ARCH}.deb
    echo "  Moving ${FILE} to ${SUBDIR}..."
    mv ${FILE} ${SUBDIR}
  done
  echo "  Updating ${ARCH} repository..."
  dpkg-scanpackages ${SUBDIR} /dev/null | grep -v -E 'Depends:[^a-z]$' > ${SUBDIR}/Packages
  gzip -f ${SUBDIR}/Packages
done

echo -e "\n\nNOTE: Warnings about 'override file' are expected\n"
