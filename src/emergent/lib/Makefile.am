include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libemergent@EMERGENT_SUFFIX@-@PACKAGE_VERSION@.la
libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LDFLAGS = -version-info @LIB_VERSION@

libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = dummy.cxx

libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/emergent/network/libemergent@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/bp/libbp@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/leabra/libleabra@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/so/libso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/cs/libcs@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la

clean-local:
	-rm -f libemergent*

