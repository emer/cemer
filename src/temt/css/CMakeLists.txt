########### css target ###############

# get the files
include(CMakeFiles.txt)

########### install files ###############

install(FILES ${css_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})

##############################################################
##  extra stuff from original Makefile.am
#
#make_css_parse:
#	bison -dtvy css_parse.y;
#	sed -e "s/union/struct/g" y.tab.c >css_parse.cpp;
#	sed -e "s/union/struct/g" y.tab.h >css_parse.hxx;
#	cmp css_parse.hxx css_parse.h || cp css_parse.hxx css_parse.h;
#
