########### maketa target ###############

set(maketa_HEADS
  ../ta/ta_stdef.h
  ../ta/taBasicAtomicInt.h
  ../ta/taAtomicInt.h
  ../ta/taString.h
  ../ta/Variant.h
  ../ta/taRefN.h

  ../ta/NameVar.h
  ../ta/MTRnd.h
  ../ta/taPtrList_impl.h
  ../ta/taPtrList.h
  ../ta/taPtrList_base.h
  ../ta/taHashEl.h
  ../ta/taHashBucket.h
  ../ta/taHashTable.h
  ../ta/taArray_impl.h
  ../ta/taPlainArray.h
  ../ta/taFixedArray_impl.h
  ../ta/taFixedArray.h
  ../ta/int_FixedArray.h
  ../ta/String_PArray.h
  ../ta/int_PArray.h
  ../ta/voidptr_PArray.h
  ../ta/NameVar_PArray.h

  ../ta/ITypedObject.h
  ../ta/TypeItem.h
  ../ta/EnumSpace.h
  ../ta/TokenSpace.h
  ../ta/MemberDefBase_List.h
  ../ta/MemberSpace.h
  ../ta/Member_List.h
  ../ta/PropertySpace.h
  ../ta/MethodSpace.h
  ../ta/TypeSpace.h
  ../ta/EnumDef.h
  ../ta/MemberDefBase.h
  ../ta/MemberDef.h
  ../ta/PropertyDef.h
  ../ta/MethodDef.h
  ../ta/TypeDef.h
  ../ta/BuiltinTypeDefs.h

  ../ta/taVersion.h
  ../ta/taMisc.h

  mta_parse.h
  mta_constr.h
  maketa.h
)

set(maketa_SRCS
#  ../ta/ta_ti.cpp
  ../ta/taBasicAtomicInt.cpp
  ../ta/taAtomicInt.cpp
  ../ta/taString.cpp
  ../ta/Variant.cpp
  ../ta/taRefN.cpp

  ../ta/NameVar.cpp
  ../ta/MTRndState.cpp
  ../ta/MTRnd.cpp
  ../ta/taPtrList_impl.cpp
  ../ta/taPtrList.cpp
  ../ta/taPtrList_base.cpp
  ../ta/taHashEl.cpp
  ../ta/taHashBucket.cpp
  ../ta/taHashTable.cpp
  ../ta/taArray_impl.cpp
  ../ta/taPlainArray.cpp
  ../ta/taFixedArray_impl.cpp
  ../ta/taFixedArray.cpp
  ../ta/int_FixedArray.cpp
  ../ta/String_PArray.cpp
  ../ta/int_PArray.cpp
  ../ta/voidptr_PArray.cpp
  ../ta/NameVar_PArray.cpp

  ../ta/ITypedObject.cpp
  ../ta/TypeItem.cpp
  ../ta/EnumSpace.cpp
  ../ta/TokenSpace.cpp
  ../ta/MemberDefBase_List.cpp
  ../ta/MemberSpace.cpp
  ../ta/Member_List.cpp
  ../ta/PropertySpace.cpp
  ../ta/MethodSpace.cpp
  ../ta/TypeSpace.cpp
  ../ta/EnumDef.cpp
  ../ta/MemberDefBase.cpp
  ../ta/MemberDef.cpp
  ../ta/PropertyDef.cpp
  ../ta/MethodDef.cpp
  ../ta/TypeDef.cpp
  ../ta/BuiltinTypeDefs.cpp
  ../ta/taVersion.cpp
  ../ta/taMisc.cpp
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
  add_definitions(-O2 -g)
endif (WIN32)

#NOTE: we don't use decorative suffix -- confusing, and there should only be one
# DON'T use EMERGENT_ADD_EXECUTABLE here!  The whole purpose of that macro is
# to NOT use it here.
add_executable(maketa ${maketa_SRCS})

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

