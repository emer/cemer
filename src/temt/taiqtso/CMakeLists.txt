########### taiqtso target ###############

# get the files
include(CMakeFiles.txt)

# nothing actually needed here because no maketa

########### install files ###############

#install(FILES  $(HEADS) $(TAHEADS) taiqtso_def.h DESTINATION /include)


##############################################################
##  extra stuff from original Makefile.am
#
#
#include_HEADERS = $(HEADS) $(TAHEADS) taiqtso_def.h
#include $(top_srcdir)/Moc.am
#include $(top_srcdir)/mfstamp.am
#
#irenderarea.moc.cpp:
#	$(QT_MOC) -DTA_USE_INVENTOR -o irenderarea.moc.cpp irenderarea.h
#
#interceptor.moc.cpp:
#	$(QT_MOC) -o interceptor.moc.cpp -I@abs_top_builddir@ -I@abs_top_builddir@/src/temt/ta interceptor.h
#
#qconsole.moc.cpp:
#	$(QT_MOC) -o qconsole.moc.cpp -I@abs_top_builddir@ -I@abs_top_builddir@/src/temt/ta qconsole.h
#
#itextbrowser.h:
#
#
#
#noinst_LTLIBRARIES = libtaiqtso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@.la
#libtaiqtso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_SOURCES = $(SRCS) $(HEADS)
#libtaiqtso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CPPFLAGS = @EMERGENT_INCLUDES@
#libtaiqtso@EMERGENT_SUFFIX@_@PACKAGE_VERSION@_la_CXXFLAGS = @OPT_LEVEL@
#
#clean-local:
#	-rm -f libtaiqtso*
#	if [ -n "`ls *\.moc\.cpp`" ];then rm `ls *\.moc\.cpp`;fi
#
