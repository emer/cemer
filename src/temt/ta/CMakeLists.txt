########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/ta)

########### running rcc qt resource compiler on ta.qrc ###############

ADD_CUSTOM_TARGET(make_ta_qrc
#  OUTPUT ta_qrc.cpp
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND rcc -o ta_qrc.cpp ta.qrc
  DEPENDS ta.qrc
  )

########### install files ###############

EMERGENT_INSTALL_HEADERS("${ta_HEADS}")

