lib_LTLIBRARIES = libta4@PDP_SUFFIX@.la
libta4@PDP_SUFFIX@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libta4@PDP_SUFFIX@_la_SOURCES = dummy.cxx

libta4@PDP_SUFFIX@_la_LIBADD =\
	@abs_top_builddir@/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@.la

if TA_GUI
libta4@PDP_SUFFIX@_la_LIBADD +=\
	@abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la
endif
