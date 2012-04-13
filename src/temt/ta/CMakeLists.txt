########### ta target ###############

# get the files
include(CMakeFiles.txt)

set(mypath ${PROJECT_SOURCE_DIR}/src/temt/ta)

CREATE_MAKETA_COMMAND(ta ${mypath} "${ta_HEADS_MAKETA}")

########### running rcc qt resource compiler on ta.qrc ###############

ADD_CUSTOM_TARGET(make_ta_qrc
#  OUTPUT ta_qrc.cpp
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND rcc -o ta_qrc.cpp ta.qrc
  DEPENDS ta.qrc
  )

########### install files ###############

install(FILES ${ta_HEADS} DESTINATION ${EMERGENT_INCLUDE_DEST})

