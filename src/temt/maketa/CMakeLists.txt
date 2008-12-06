########### maketa target ###############

set(maketa_HEADS
  ../ta/ta_stdef.h
  ../ta/ta_platform.h
  ../ta/ta_string.h
  ../ta/ta_variant.h
  ../ta/ta_mtrnd.h
  ../ta/ta_list.h
  ../ta/ta_type.h
  mta_parse.h
  mta_constr.h
  maketa.h
)

set(maketa_SRCS
  ../ta/ta_ti.cpp
  ../ta/ta_platform.cpp
  ../ta/ta_string.cpp
  ../ta/ta_variant.cpp
  ../ta/ta_mtrnd.cpp
  ../ta/ta_list.cpp
  ../ta/ta_type.cpp
  mta_parse.cpp
  mta_lex.cpp
  mta_constr.cpp
  mta_gendoc.cpp
  maketa.cpp
)

set(maketa_FILES ${maketa_HEADS} ${maketa_SRCS})

 add_definitions(-DNO_TA_BASE)
# note: special definitions for compiling maketa: always optimized
if (WIN32)
# :( /O2 is incompatible with /RTC1 that gets set automatically
#  add_definitions(/O2)
else (WIN32)
  add_definitions(-O2 -g)
endif (WIN32)

add_executable(maketa ${maketa_SRCS})

#no target_link_libraries(maketa ${QT_LIBRARIES})

########### running bison on mta_parse.y ###############

# note: change TARGET -> COMMAND and uncomment OUTPUT to get auto-building
ADD_CUSTOM_TARGET(make_mta_parse
#  OUTPUT mta_parse.cpp mta_parse.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND bison -dtvy mta_parse.y
  COMMAND sed -e "s/union/struct/g" y.tab.c >mta_parse.cpp
  COMMAND sed -e "s/union/struct/g" y.tab.h >mta_parse.hxx
  COMMAND cmp -s mta_parse.hxx mta_parse.h || cp mta_parse.hxx mta_parse.h
  DEPENDS mta_parse.y
  )

########### install files ###############

install(TARGETS maketa
  RUNTIME DESTINATION bin
)

