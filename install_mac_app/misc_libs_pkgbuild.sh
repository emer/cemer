#!/bin/bash

# note: edit package names here
misclibspkg=misclibs_mac64_8_0
tmpdir=./

bundir=${tmpdir}/tmp_${misclibspkg}

mkdir ${bundir}
mkdir ${bundir}/include
mkdir ${bundir}/lib
mkdir ${bundir}/lib/pkgconfig
mkdir ${bundir}/bin

# jpeg:
/bin/cp -av /usr/local/include/jpeglib.h ${bundir}/include/
/bin/cp -av /usr/local/lib/libjpeg.62.0.0.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libjpeg.62.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libjpeg.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libjpeg.la ${bundir}/lib/
/bin/cp -av /usr/local/lib/libjpeg.a ${bundir}/lib/
#/bin/cp -av /usr/local/lib/pkgconfig/jpeg.pc ${bundir}/lib/pkgconfig/
#/bin/cp -av /usr/local/bin/jpeg-config ${bundir}/bin/

# ode:
/bin/cp -av /usr/local/include/ode ${bundir}/include/
/bin/cp -av /usr/local/lib/libode.6.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libode.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libode.la ${bundir}/lib/
/bin/cp -av /usr/local/lib/libode.a ${bundir}/lib/
/bin/cp -av /usr/local/lib/pkgconfig/ode.pc ${bundir}/lib/pkgconfig/
/bin/cp -av /usr/local/bin/ode-config ${bundir}/bin/

# gsl:
/bin/cp -av /usr/local/include/gsl ${bundir}/include/
/bin/cp -av /usr/local/lib/libgsl.0.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libgsl.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libgsl.la ${bundir}/lib/
/bin/cp -av /usr/local/lib/libgsl.a ${bundir}/lib/
/bin/cp -av /usr/local/lib/libgslcblas.0.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libgslcblas.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libgslcblas.la ${bundir}/lib/
/bin/cp -av /usr/local/lib/libgslcblas.a ${bundir}/lib/
/bin/cp -av /usr/local/lib/pkgconfig/gsl.pc ${bundir}/lib/pkgconfig/
/bin/cp -av /usr/local/bin/gsl-config ${bundir}/bin/

# sndfile:
/bin/cp -av /usr/local/include/sndfile.h ${bundir}/include/
/bin/cp -av /usr/local/lib/libsndfile.1.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libsndfile.dylib ${bundir}/lib/
/bin/cp -av /usr/local/lib/libsndfile.la ${bundir}/lib/
/bin/cp -av /usr/local/lib/libsndfile.a ${bundir}/lib/
/bin/cp -av /usr/local/lib/pkgconfig/sndfile.pc ${bundir}/lib/pkgconfig/
/bin/cp -av /usr/local/bin/sndfile-* ${bundir}/bin/

pkgbuild --root ${bundir} --install-location /usr/local --identifier edu.colorado.ccnlab.emergent.misclibs --version 14.0 ./${misclibspkg}.pkg

./make_dmg_pkg.sh $misclibspkg

/bin/rm -rf ${bundir}
