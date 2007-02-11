include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libtacss@PDP_SUFFIX@-@PACKAGE_VERSION@.la
libtacss@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LDFLAGS = -version-info @LIB_VERSION@
libtacss@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libtacss@PDP_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = dummy.cxx
libtacss@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@_@PACKAGE_VERSION@.la

if TA_GUI
libtacss@PDP_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD +=\
	@abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@_@PACKAGE_VERSION@.la
endif



