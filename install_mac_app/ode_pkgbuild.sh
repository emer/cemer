#!/bin/bash

# note: edit package names here
odepkg=ode_mac64_14_0
tmpdir=./

bundir=${tmpdir}/tmp_${odepkg}

mkdir ${bundir}
mkdir ${bundir}/include
mkdir ${bundir}/lib
mkdir ${bundir}/lib/pkgconfig
mkdir ${bundir}/bin

/bin/cp -av /usr/local/include/ode ${bundir}/include/
/bin/cp -av /usr/local/lib/libode.6.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libode.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libode.la ${bundir}/lib/
/bin/cp -av /usr/local/lib/libode.a ${bundir}/lib/
/bin/cp -av /usr/local/lib/pkgconfig/ode.pc ${bundir}/lib/pkgconfig/
/bin/cp -av /usr/local/bin/ode-config ${bundir}/bin/

pkgbuild --root ${bundir} --install-location /usr/local --identifier edu.colorado.ccnlab.emergent.ode --version 14.0 ./${odepkg}.pkg

./make_dmg_pkg.sh $odepkg

/bin/rm -rf ${bundir}
