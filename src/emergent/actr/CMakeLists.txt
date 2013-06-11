########### emergent actr target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/emergent/actr)

CREATE_MAKETA_COMMAND(actr ${mypath} "${actr_HEADS_MAKETA}")

########### running bison on actr_parse.y ###############

# note: change TARGET -> COMMAND and uncomment OUTPUT to get auto-building
ADD_CUSTOM_TARGET(make_actr_parse
#  OUTPUT actr_parse.cpp actr_parse.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND bison -dtvy actr_parse.y
  COMMAND sed -e "s/union/struct/g" y.tab.c >actr_parse.cpp
  COMMAND sed -e "s/union/struct/g" y.tab.h >actr_parse.hxx
  COMMAND cmp -s actr_parse.hxx actr_parse.h || cp actr_parse.hxx actr_parse.h
  DEPENDS actr_parse.y
  )

########### install files ###############

EMERGENT_INSTALL_HEADERS("${actr_HEADS}")

