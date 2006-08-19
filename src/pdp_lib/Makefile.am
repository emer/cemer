include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libpdp4@PDP_SUFFIX@.la

libpdp4@PDP_SUFFIX@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libpdp4@PDP_SUFFIX@_la_SOURCES = dummy.cxx

if LIBTA
libpdp4@PDP_SUFFIX@_la_LIBADD =\
	@abs_top_builddir@/src/pdp/libpdp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/bp/libbp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/leabra/libleabra@PDP_SUFFIX@.la
else
libpdp4@PDP_SUFFIX@_la_LIBADD =\
	@abs_top_builddir@/src/tamisc/libtamisc@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/css/libcss@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/ta/libta@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/pdp/libpdp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/bp/libbp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/leabra/libleabra@PDP_SUFFIX@.la

if TA_GUI
libpdp4@PDP_SUFFIX@_la_LIBADD +=\
	@abs_top_builddir@/src/taiqtso/libtaiqtso@PDP_SUFFIX@.la
endif

endif
