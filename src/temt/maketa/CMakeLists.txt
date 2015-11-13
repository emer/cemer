########### maketa target ###############

set(maketa_HEADS
  ../ta_core/ta_stdef.h
  ../ta_core/taBasicAtomicInt.h
  ../ta_core/taAtomicInt.h
  ../ta_core/taString.h
  ../ta_core/Variant.h
  ../ta_core/taRefN.h

  ../ta_core/NameVar.h
  ../ta_core/taPtrList_impl.h
  ../ta_core/taPtrList.h
  ../ta_core/taPtrList_base.h
  ../ta_core/taHashEl.h
  ../ta_core/taHashBucket.h
  ../ta_core/taHashTable.h
  ../ta_core/taArray_impl.h
  ../ta_core/taPlainArray.h
  ../ta_core/taFixedArray_impl.h
  ../ta_core/taFixedArray.h
  ../ta_core/int_FixedArray.h
  ../ta_core/String_PArray.h
  ../ta_core/int_PArray.h
  ../ta_core/voidptr_PArray.h
  ../ta_core/NameVar_PArray.h

  ../ta_core/ITypedObject.h
  ../ta_core/TypeItem.h
  ../ta_core/EnumSpace.h
  ../ta_core/TokenSpace.h
  ../ta_core/MemberDefBase_List.h
  ../ta_core/MemberSpace.h
  ../ta_core/Member_List.h
  ../ta_core/PropertySpace.h
  ../ta_core/MethodSpace.h
  ../ta_core/TypeSpace.h
  ../ta_core/EnumDef.h
  ../ta_core/MemberDefBase.h
  ../ta_core/MemberDef.h
  ../ta_core/PropertyDef.h
  ../ta_core/MethodDef.h
  ../ta_core/TypeDef.h
  ../ta_core/BuiltinTypeDefs.h

  ../ta_core/taVersion.h
  ../ta_core/taMisc.h

  mta_parse.h
  mta_constr.h
  maketa.h
)

set(maketa_SRCS
#  ../ta_core/ta_ti.cpp
  ../ta_core/taBasicAtomicInt.cpp
  ../ta_core/taAtomicInt.cpp
  ../ta_core/taString.cpp
  ../ta_core/Variant.cpp
  ../ta_core/taRefN.cpp

  ../ta_core/NameVar.cpp
  ../ta_core/taPtrList_impl.cpp
  ../ta_core/taPtrList.cpp
  ../ta_core/taPtrList_base.cpp
  ../ta_core/taHashEl.cpp
  ../ta_core/taHashBucket.cpp
  ../ta_core/taHashTable.cpp
  ../ta_core/taArray_impl.cpp
  ../ta_core/taPlainArray.cpp
  ../ta_core/taFixedArray_impl.cpp
  ../ta_core/taFixedArray.cpp
  ../ta_core/int_FixedArray.cpp
  ../ta_core/String_PArray.cpp
  ../ta_core/int_PArray.cpp
  ../ta_core/voidptr_PArray.cpp
  ../ta_core/NameVar_PArray.cpp

  ../ta_core/ITypedObject.cpp
  ../ta_core/TypeItem.cpp
  ../ta_core/EnumSpace.cpp
  ../ta_core/TokenSpace.cpp
  ../ta_core/MemberDefBase_List.cpp
  ../ta_core/MemberSpace.cpp
  ../ta_core/Member_List.cpp
  ../ta_core/PropertySpace.cpp
  ../ta_core/MethodSpace.cpp
  ../ta_core/TypeSpace.cpp
  ../ta_core/EnumDef.cpp
  ../ta_core/MemberDefBase.cpp
  ../ta_core/MemberDef.cpp
  ../ta_core/PropertyDef.cpp
  ../ta_core/MethodDef.cpp
  ../ta_core/TypeDef.cpp
  ../ta_core/BuiltinTypeDefs.cpp
  ../ta_core/taVersion.cpp
  ../ta_core/taMisc.cpp
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
# :( /O2 is incompatible with /RTC1 that gets set automatically -- so can't do this!
#  add_definitions(/O2)
else (WIN32)
#  add_definitions(-O2 -g)
endif (WIN32)

#NOTE: we don't use decorative suffix -- confusing, and there should only be one
# DON'T use EMERGENT_ADD_EXECUTABLE here!  The whole purpose of that macro is
# to NOT use it here.
add_executable(maketa ${maketa_SRCS})

if (QT_USE_5)
  set(CMAKE_PREFIX_PATH ${QTDIR}/lib/cmake)
  find_package(Qt5Core)
  include_directories(${Qt5Core_INCLUDE_DIRS})
  target_link_libraries(maketa ${Qt5Core_LIBRARIES})
else (QT_USE_5)
  find_package(Qt4 REQUIRED QtCore)
  set(QT_USE_QT3SUPPORT 0)
  include_directories(${QT_INCLUDES})
  include(${QT_USE_FILE})
  target_link_libraries(maketa ${QT_LIBRARIES})
endif (QT_USE_5)  

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
  OPTIONAL #TEMP
)

