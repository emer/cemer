include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libpdp4@PDP_SUFFIX@.la

libpdp4@PDP_SUFFIX@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libpdp4@PDP_SUFFIX@_la_SOURCES = dummy.cxx

libpdp4@PDP_SUFFIX@_la_LIBADD =\
	@abs_top_builddir@/src/pdp/libpdp@PDP_SUFFIX@.la \
	@abs_top_builddir@/src/bp/libbp@PDP_SUFFIX@.la
