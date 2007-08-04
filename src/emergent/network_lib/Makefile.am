include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libemergent@PDP_SUFFIX@-@PACKAGE_VERSION@.la
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LDFLAGS = -version-info @LIB_VERSION@

libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = dummy.cxx

if LIBTA
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/emergent/network/libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/bp/libbp@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/leabra/libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/so/libso@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/cs/libcs@PDP_SUFFIX@_@PACKAGE_VERSION@.la
else
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/temt/css/libcss@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/temt/ta/libta@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/network/libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/bp/libbp@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/leabra/libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/so/libso@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/cs/libcs@PDP_SUFFIX@_@PACKAGE_VERSION@.la

if TA_GUI
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD +=\
	@abs_top_builddir@/src/temt/taiqtso/libtaiqtso@PDP_SUFFIX@_@PACKAGE_VERSION@.la
endif
endif
