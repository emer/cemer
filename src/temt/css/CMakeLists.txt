########### css target ###############

# get the files
include(CMakeFiles.txt)

########### install files ###############

#install(FILES  $(HEADS) $(TAHEADS) DESTINATION /include)


##############################################################
##  extra stuff from original Makefile.am
#
#
#
#css_qtconsole.moc.cpp:
#	$(QT_MOC) -o css_qtconsole.moc.cpp -I@abs_top_builddir@ -I@abs_top_builddir@/src/css -I@abs_top_builddir@/src/temt/ta css_qtconsole.h
#
#make_css_parse:
#	bison -dtvy css_parse.y;
#	sed -e "s/union/struct/g" y.tab.c >css_parse.cpp;
#	sed -e "s/union/struct/g" y.tab.h >css_parse.hxx;
#	cmp css_parse.hxx css_parse.h || cp css_parse.hxx css_parse.h;
#
