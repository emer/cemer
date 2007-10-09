include $(top_srcdir)/mfstamp.am

lib_LTLIBRARIES = libtemt@EMERGENT_SUFFIX@-@PACKAGE_VERSION@.la

libtemt@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LDFLAGS = -version-info @LIB_VERSION@

# this breaks linking on mac!!!!
# See http://www.nabble.com/warning-with-the-linker:-undefined-symbols-t2904530.html
# for infor on -no-undefied
#libtemt@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LDFLAGS = -version-info @LIB_VERSION@ -no-undefined

libtemt@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES =

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libtemt@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = dummy.cxx
libtemt@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD =\
	@abs_top_builddir@/src/temt/css/libcss@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la \
	@abs_top_builddir@/src/temt/ta/libta@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la

if TA_GUI
libtemt@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_LIBADD +=\
	@abs_top_builddir@/src/temt/taiqtso/libtaiqtso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
endif

clean-local:
	-rm -f libtemt*
	if [ -n "`ls *\.moc\.cpp`" ];then rm `ls *\.moc\.cpp`;fi
