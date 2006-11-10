include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libpdp@PDP_SUFFIX@@PDP_VERSION@.la
libpdp@PDP_SUFFIX@@PDP_VERSION@_la_LDFLAGS = -version-info @AX_LIB_MAJOR_VERSION@:@AX_LIB_MINOR_VERSION@:@AX_LIB_POINT_VERSION@
libpdp@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libpdp@PDP_SUFFIX@@PDP_VERSION@_la_SOURCES = dummy.cxx

if LIBTA
libpdp@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/pdp/libpdp@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/bp/libbp@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/leabra/libleabra@PDP_SUFFIX@@PDP_VERSION@.la
else
libpdp@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/pdp/libpdp@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/bp/libbp@PDP_SUFFIX@@PDP_VERSION@.la \
	@abs_top_builddir@/src/leabra/libleabra@PDP_SUFFIX@@PDP_VERSION@.la

if TA_GUI
libpdp@PDP_SUFFIX@@PDP_VERSION@_la_LIBADD +=\
	@abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@@PDP_VERSION@.la
endif
endif
