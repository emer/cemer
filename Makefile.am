
# TODO: Clean up xml files created by gendoc

CLEAN_FILES = 

EXTRA_DIST =\
	src/ta/ta_global.h \
	src/ta/ta_maketa_defs.h \
	src/ta/qtdefs.h \
	src/css/css_def.h \
	src/pdp/pdp_def.h \
	src/bp/bp_def.h \
	src/leabra/leabra_def.h \
	src/so/so_def.h \
	spec.tmpl.in 

dist_pkgdata_DATA = AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README

# SUBDIRS tells the autotools which directories have Makefile.am's in them.
# If you remove/add one of SUBDIRS, be sure to do the same at the bottom of configure.ac

DIST_SUBDIRS =\
	src/maketa \
	src/taiqtso \
	src/ta \
	src/css \
	src/ta_lib \
        src/css_bin \
	src/pdp \
	src/bp \
	src/leabra \
	src/so \
	src/pdp_lib \
	src/pdp_bin \
	prog_lib \
	css_stdlib \
	test

SUBDIRS =\
	src/maketa \
	@WANT_TAIQTSO@ \
	src/ta \
	src/css \
	@WANT_LIBTA@ \
	@WANT_CSS_BIN@ \
	src/pdp \
	src/bp \
	src/leabra \
	src/so \
	src/pdp_lib \
	src/pdp_bin \
	prog_lib \
	css_stdlib \
	test



EXTRA_DIST += src/ta/ios-g++-3.1

#For make rpm
if RPM

@INCLUDE_AMINCLUDE_AM@
#include aminclude.am

EXTRA_DIST += aminclude.am spec.tmpl.in


	*do-stamp* \
	PDP++-3.5.spec \
	RPMChangeLog 

DISTCLEANFILES =\
	aminclude.am \
	do-mfstamp-am \
	mfstamp \
	do-mfstamp \
	manual/do-mfstamp \
	src/taiqtso/do-mfstamp \
	src/ta/do-mfstamp \
	src/css/do-mfstamp \
	src/css_bin/do-mfstamp \
	src/pdp/do-mfstamp \
	src/leabra/do-mfstamp \
	src/bp/do-mfstamp \
	src/leabra/do-mfstamp \
	src/so/do-mfstamp \
	src/maketa/mfstamp \
	src/maketa/do-mfstamp \
	src/ta/ta_TA.ccx

endif

# These actions occur immediately before a distribution tarball is created.
dist-hook:
	rm -fr $(distdir)/src/ta/ios-g++-3.1/.svn

# 	mkdir $(distdir)/prog_lib $(distdir)/css_stdlib $(distdir)/tests
# 	cp -pR $(srcdir)/prog_lib $(distdir)/prog_lib
# 	cp -pR $(srcdir)/css_stdlib $(distdir)/css_stdlib
# 	cp -pR $(srcdir)/test $(distdir)/test
# 	rm -fr $(distdir)/prog_lib/.svn $(distdir)/css_stdlib/.svn $(distdir)/test/.svn

# Count the number of lines in *.h and *.cpp files
linecount:
	@ wc --lines `find -name "*.cpp" -printf '%p ' -o -name "*.h" -printf '%p '` | grep total

# Update our changelog from the subversion repository
# This has prerequisites. See:
# http://grey.colorado.edu/cgi-bin/trac.cgi/wiki/CreatingaGNUcompliantChangeLogfromSubversioncommits
changelog:
	svn log --xml --verbose | xsltproc /home/mingus/opt/svn2cl-0.3/svn2cl.xsl - > ChangeLog

# todo: need to make some dependency on this so it gets activated..
${abs_top_builddir}/src/ta/ta_TA_type.h: ${abs_top_builddir}/src/maketa/maketa
	$(MAKE) force_ta

force_ta:
	cd ${abs_top_builddir}/src/ta && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/pdp && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/bp && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/leabra && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/so && ${MAKE} force_ta

gendoc:
	cd ${abs_top_builddir}/src/ta && ${MAKE} gendoc
	cd ${abs_top_builddir}/src/pdp && ${MAKE} gendoc
	cd ${abs_top_builddir}/src/bp && ${MAKE} gendoc
	cd ${abs_top_builddir}/src/leabra && ${MAKE} gendoc

do_ta:
	cd ${abs_top_builddir}/src/ta && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/pdp && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/bp && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/leabra && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/so && ${MAKE} do_ta

ifnames:
	ifnames `find -name "*.cpp" -printf '%p ' -o -name "*.h" -printf '%p '` > ifnames.log
	echo "Defines printed to ifnames.log"

spec.tmpl.in:


# You net to export/setenv ${DEBEMAIL}
deb:
	if test -e trunk.spec; then rm -f trunk.spec;fi
	checkinstall -D -y --install=no --pkgname=${PACKAGE_NAME}-nightly --pkgversion=${PACKAGE_VERSION} --arch=${build_cpu} --pkglicense=GPL --maintainer=${DEBEMAIL} --reset-uids=yes --pakdir=/usr/local/apt/archives/binary --requires=pdp-qt,pdp-coin,pdp-soqt --include=/usr/local/lib/libpdp-${PACKAGE_VERSION}.so.0,/usr/local/lib/libtacss-${PACKAGE_VERSION}.so.0
	dpkg-scanpackages /usr/local/apt/archives/binary /dev/null | gzip -9c > /usr/local/apt/archives/binary/Packages.gz


rpm2:
	if test -e trunk.spec; then rm -f trunk.spec;fi
	checkinstall -R -y --install=no --pkgname=${PACKAGE_NAME}-nightly --pkgversion=${PACKAGE_VERSION} --arch=${build_cpu} --pkglicense=GPL --maintainer=${DEBEMAIL} --reset-uids=yes --pakdir /usr/local/yum --requires=pdp-qt,pdp-coin,pdp-soqt --include=/usr/local/lib/libpdp-${PACKAGE_VERSION}.so.0,/usr/local/lib/libtacss-${PACKAGE_VERSION}.so.0
	createrepo /usr/local/yum

packages: deb rpm2
