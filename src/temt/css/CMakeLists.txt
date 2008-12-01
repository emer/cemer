########### css target ###############

# get the files
include(CMakeFiles.txt)

########### running bison on css_parse.y ###############

# note: change TARGET -> COMMAND and uncomment OUTPUT to get auto-building
ADD_CUSTOM_TARGET(make_css_parse
#  OUTPUT css_parse.cpp css_parse.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND bison -dtvy css_parse.y
  COMMAND sed -e "s/union/struct/g" y.tab.c >css_parse.cpp
  COMMAND sed -e "s/union/struct/g" y.tab.h >css_parse.hxx
  COMMAND cmp -s css_parse.hxx css_parse.h || cp css_parse.hxx css_parse.h
  DEPENDS css_parse.y
  )

########### install files ###############

install(FILES ${css_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})

