CLEAN_FILES =
EXTRA_DIST =

dist_pkgdata_DATA = AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README

# SUBDIRS tells the autotools which directories have Makefile.am's in them.
# If you remove/add one of SUBDIRS, be sure to do the same at the bottom of configure.ac

DIST_SUBDIRS =\
	src/maketa \
	src/taiqtso \
	src/ta \
	src/css \
	src/tamisc \
	src/ta_lib \
        src/css_bin \
	src/pdp \
	src/bp \
	src/leabra \
	src/pdp_lib \
	src/pdp_bin \
	manual

SUBDIRS =\
	src/maketa \
	@WANT_TAIQTSO@ \
	src/ta \
	src/css \
	src/tamisc \
	src/ta_lib \
	src/css_bin \
	src/pdp \
	src/bp \
	src/leabra \
	src/pdp_lib \
	src/pdp_bin \
	manual

EXTRA_DIST += src/ta/ios-g++-3.1

#For make rpm
if RPM

@INCLUDE_AMINCLUDE_AM@
#include aminclude.am

EXTRA_DIST += aminclude.am spec.tmpl.in

CLEANFILES =\
	*do-stamp* \
	PDP++-4.0.0.spec \
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
	src/tamisc/do-mfstamp \
	src/css_bin/do-mfstamp \
	src/pdp/do-mfstamp \
	src/leabra/do-mfstamp \
	src/bp/do-mfstamp \
	src/leabra/do-mfstamp \
	src/maketa/mfstamp \
	src/maketa/do-mfstamp \
	src/ta/ta_TA.ccx

endif


# These actions occur immediately before a distribution tarball is created.

dist-hook:
	rm -fr $(distdir)/src/ta/ios-g++-3.1/.svn
	cp -cp $(srcdir)/src/ta/taglobal.h \
		$(srcdir)/src/ta/qtdefs.h\
		$(distdir)/src/ta
	cp -cp $(srcdir)/src/tamisc/tamisc_maketa_defs.h \
		$(distdir)/src/tamisc/tamisc_maketa_defs.h
# 	cp -cp $(srcdir)/src/ta/ta_maketa_defs.h \
# 		$(distdir)/src/ta/ta_maketa_defs.h
#	cp -p $(srcdir)/spec.tmpl.in $(distdir)/spec.tmpl.in

# Count the number of lines in *.h and *.cpp files
linecount:
	@ wc --lines `find -name "*.cpp" -printf '%p ' -o -name "*.h" -printf '%p '` | grep total

# Update our changelog from the subversion repository
# This has prerequisites. See:
# http://grey.colorado.edu/cgi-bin/trac.cgi/wiki/CreatingaGNUcompliantChangeLogfromSubversioncommits
changelog:
	svn log --xml --verbose | xsltproc /home/mingus/opt/svn2cl-0.3/svn2cl.xsl - > ChangeLog

force_ta:
	cd ${abs_top_builddir}/src/ta && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/tamisc && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/pdp && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/bp && ${MAKE} force_ta
	cd ${abs_top_builddir}/src/leabra && ${MAKE} force_ta

do_ta:
	cd ${abs_top_builddir}/src/ta && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/tamisc && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/pdp && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/bp && ${MAKE} do_ta
	cd ${abs_top_builddir}/src/leabra && ${MAKE} do_ta

ifnames:
	ifnames `find -name "*.cpp" -printf '%p ' -o -name "*.h" -printf '%p '` > ifnames.log
	echo "Defines printed to ifnames.log"
