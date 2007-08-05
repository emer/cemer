CLEAN_FILES =  # TODO: Clean up xml files created by gendoc

EXTRA_DIST =\
	src/temt/ta/ta_global.h \
	src/temt/ta/ta_maketa_defs.h \
	src/temt/ta/qtdefs.h \
	src/temt/css/css_def.h \
	src/emergent/network/pdp_def.h \
	src/emergent/bp/bp_def.h \
	src/emergent/leabra/leabra_def.h \
	src/emergent/so/so_def.h \
	src/emergent/cs/cs_def.h \
	spec.tmpl.in \
	src/temt/ta/files.mta \
	src/emergent/network/files.mta \
	src/emergent/bp/files.mta \
	src/emergent/leabra/files.mta \
	src/emergent/so/files.mta \
	src/emergent/cs/files.mta \
	src/temt/ta/home.xpm \
	src/temt/ta/ortho.xpm \
	src/temt/ta/perspective.xpm \
	src/temt/ta/pick.xpm \
	src/temt/ta/print.xpm \
	src/temt/ta/seek.xpm \
	src/temt/ta/set_home.xpm \
	src/temt/ta/snapshot.xpm \
	src/temt/ta/view.xpm \
	src/temt/ta/view_all.xpm \
	src/temt/ta/x.xpm \
	src/temt/ta/y.xpm \
	src/temt/ta/z.xpm

dist_pkgdata_DATA = AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README

# SUBDIRS tells the autotools which directories have Makefile.am's in them.
# If you remove/add one of SUBDIRS, be sure to do the same at the bottom of configure.ac

DIST_SUBDIRS =\
	src/temt/maketa \
	src/temt/taiqtso \
	src/temt/ta \
	src/temt/css \
	src/temt/lib \
        src/temt/css_bin \
	src/emergent/bp \
	src/emergent/leabra \
	src/emergent/so \
	src/emergent/cs \
	src/emergent/network \
	src/emergent/lib \
	src/emergent/bin \
	plugins \
	prog_lib \
	css_stdlib \
	test \
	demo/bp \
	demo/cs \
	demo/leabra \
	demo/so \
	demo/virt_env

SUBDIRS =\
	src/temt/maketa \
	@WANT_TAIQTSO@ \
	src/temt/ta \
	src/temt/css \
	@WANT_LIBTA@ \
	@WANT_CSS_BIN@ \
	src/emergent/network \
	src/emergent/bp \
	src/emergent/leabra \
	src/emergent/so \
	src/emergent/cs \
	src/emergent/lib \
	src/emergent/bin \
	@WANT_PLUGINS@ \
	prog_lib \
	css_stdlib \
	test \
	demo/bp \
	demo/cs \
	demo/leabra \
	demo/so \
	demo/virt_env

EXTRA_DIST += src/temt/ta/ios-g++-3.1 @WANT_PLUGINS@

#For make rpm
if RPM

@INCLUDE_AMINCLUDE_AM@
#include aminclude.am

EXTRA_DIST +=\
	aminclude.am \
	spec.tmpl.in \
	*do-stamp* \
	PDP++-3.5.spec \
	RPMChangeLog 

DISTCLEANFILES =\
	aminclude.am \
	do-mfstamp-am \
	mfstamp \
	do-mfstamp \
	manual/do-mfstamp \
	src/temt/taiqtso/do-mfstamp \
	src/temt/ta/do-mfstamp \
	src/temt/css/do-mfstamp \
	src/temt/css_bin/do-mfstamp \
	src/emergent/do-mfstamp \
	src/emergent/leabra/do-mfstamp \
	src/emergent/bp/do-mfstamp \
	src/emergent/leabra/do-mfstamp \
	src/emergent/so/do-mfstamp \
	src/emergent/cs/do-mfstamp \
	src/temt/maketa/mfstamp \
	src/temt/maketa/do-mfstamp \
	src/temt/ta/ta_TA.ccx

endif

# These actions occur immediately before a distribution tarball is created.
dist-hook:
	rm -fr $(distdir)/src/temt/ta/ios-g++-3.1/.svn
	cp -pR $(srcdir)/plugins $(distdir)/plugins
	find $(distdir)/plugins -type d -name .svn -print | xargs -t rm -fr

# Count the number of lines in *.h and *.cpp files
linecount:
	@ wc --lines `find -name "*.cpp" -printf '%p ' -o -name "*.h" -printf '%p '` | grep total

# Update our changelog from the subversion repository
# This has prerequisites. See:
# http://grey.colorado.edu/cgi-bin/trac.cgi/wiki/CreatingaGNUcompliantChangeLogfromSubversioncommits
# TODO: Update above url
changelog:
	svn log --xml --verbose | xsltproc /home/mingus/opt/svn2cl-0.3/svn2cl.xsl - > ChangeLog

# todo: need to make some dependency on this so it gets activated..
${abs_top_builddir}/src/temt/ta/ta_TA_type.h: ${abs_top_builddir}/src/temt/maketa/maketa
	$(MAKE) force_ta

force_ta:
	cd ${abs_top_builddir}/src/temt/ta && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/emergent/network && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/emergent/bp && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/emergent/leabra && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/emergent/so && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/emergent/cs && ${MAKE} force_ta

gendoc:
	cd ${abs_top_builddir}/src/temt/ta && ${MAKE} gendoc
	cd ${abs_top_builddir}/src/emergent/network&& ${MAKE} gendoc
	cd ${abs_top_builddir}/src/emergent/bp && ${MAKE} gendoc
	cd ${abs_top_builddir}/src/emergent/leabra && ${MAKE} gendoc
	cd ${abs_top_builddir}/src/emergent/so && ${MAKE} gendoc
	cd ${abs_top_builddir}/src/emergent/cs && ${MAKE} gendoc

do_ta:
	cd ${abs_top_builddir}/src/temt/ta && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/emergent/network&& ${MAKE} do_ta
	cd ${abs_top_builddir}/src/emergent/bp && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/emergent/leabra && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/emergent/so && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/emergent/cs && ${MAKE} do_ta

ifnames:
	ifnames `find -name "*.cpp" -printf '%p ' -o -name "*.h" -printf '%p '` > ifnames.log
	echo "Defines printed to ifnames.log"

spec.tmpl.in:

# You net to export/setenv ${DEBEMAIL}
deb:
	if test -e trunk.spec; then rm -f trunk.spec;fi
	checkinstall -D -y --install=no --pkgname=${PACKAGE_NAME} --pkgversion=${PACKAGE_VERSION} --arch=${build_cpu} --pkglicense=GPL --maintainer=${DEBEMAIL} --reset-uids=yes --pakdir=/usr/local/apt/binary --requires=emergent-qt,emergent-coin,emergent-soqt,libode0c2,libode0-dev,libsimage20c2,libsimage-dev,libsndfile1,libsndfile1-dev,libgsl0,libgsl0-dev --include=/usr/local/lib/libemergent-${PACKAGE_VERSION}.so.0,/usr/local/lib/libtacss-${PACKAGE_VERSION}.so.0
	cd /usr/local/apt && dpkg-scanpackages binary /dev/null | gzip -9c > binary/Packages.gz

rpm2:
	if test -e trunk.spec; then rm -f trunk.spec;fi
	checkinstall -R -y --install=no --pkgname=${PACKAGE_NAME} --pkgversion=${PACKAGE_VERSION} --arch=${build_cpu} --pkglicense=GPL --maintainer=${DEBEMAIL} --reset-uids=yes --pakdir /usr/local/yum --requires=emergent-qt,emergent-coin,emergent-soqt,emergent-soqt,emergent-simage,ode,ode-devel,gsl  --include=/usr/local/lib/libemergent-${PACKAGE_VERSION}.so.0,/usr/local/lib/libtacss-${PACKAGE_VERSION}.so.0
	createrepo /usr/local/yum

packages: deb rpm2
