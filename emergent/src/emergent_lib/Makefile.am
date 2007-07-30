include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libemergent@PDP_SUFFIX@-@PACKAGE_VERSION@.la
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LDFLAGS = -version-info @LIB_VERSION@

libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = dummy.cxx

if LIBTA
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/emergent/libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/bp/libbp@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/leabra/libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/so/libso@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/cs/libcs@PDP_SUFFIX@_@PACKAGE_VERSION@.la
else
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/emergent/libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/bp/libbp@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/leabra/libleabra@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/so/libso@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/cs/libcs@PDP_SUFFIX@_@PACKAGE_VERSION@.la

if TA_GUI
libemergent@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD +=\
	@abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@_@PACKAGE_VERSION@.la
endif
endif
