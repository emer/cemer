#####################################
##	MAKETA suport for cmake

SET(MAKETA_FLAGS -hx -css -cpp=\"g++ -E\"
-I${PROJECT_SOURCE_DIR}/src/temt/taiqtso
-I${PROJECT_SOURCE_DIR}/src/temt/ta/ios-g++-3.1
-I${PROJECT_SOURCE_DIR}/src/temt/ta
-I${PROJECT_SOURCE_DIR}/src/temt/css
-I${PROJECT_SOURCE_DIR}/src/emergent/network
-I${PROJECT_SOURCE_DIR}/src/emergent/leabra
-I${PROJECT_SOURCE_DIR}/src/emergent/bp
)

MACRO (MAKETA_GET_INC_DIRS _MAKETA_INC_DIRS)
   SET(${_MAKETA_INC_DIRS})
   GET_DIRECTORY_PROPERTY(_inc_DIRS INCLUDE_DIRECTORIES)

   FOREACH(_current ${_inc_DIRS})
      SET(${_MAKETA_INC_DIRS} ${${_MAKETA_INC_DIRS}} "-I${_current}")
   ENDFOREACH(_current ${_inc_DIRS})
ENDMACRO(MAKETA_GET_INC_DIRS)

# this is critical for allowing dependencies to work out, and for compiling w/out extra load
MACRO (SET_TA_PROPS ta_name path)
  SET_SOURCE_FILES_PROPERTIES(${path}/${ta_name}_TA.cpp
    PROPERTIES COMPILE_FLAGS "-O0 -g0"
  )
  SET_SOURCE_FILES_PROPERTIES(${path}/${ta_name}_TA.cpp
    PROPERTIES GENERATED 1
  )
ENDMACRO(SET_TA_PROPS)

MACRO (CREATE_MAKETA_COMMAND ta_name path maketa_headers)
  MAKETA_GET_INC_DIRS(maketa_includes)

  ADD_CUSTOM_COMMAND(
    OUTPUT ${ta_name}_TA.ccx ${ta_name}_TA_type.hx ${ta_name}_TA_inst.hx
    WORKING_DIRECTORY ${path}
    COMMAND maketa ${MAKETA_FLAGS} ${maketa_includes} ${ta_name} ${maketa_headers}
    DEPENDS ${maketa_headers}
  )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${ta_name}_TA.cpp ${ta_name}_TA_type.h ${ta_name}_TA_inst.h 
    WORKING_DIRECTORY ${path}
    COMMAND cmp ${ta_name}_TA.ccx  ${ta_name}_TA.cpp || cp  ${ta_name}_TA.ccx  ${ta_name}_TA.cpp\; cmp ${ta_name}_TA_type.hx  ${ta_name}_TA_type.h || cp  ${ta_name}_TA_type.hx  ${ta_name}_TA_type.h\; cmp ${ta_name}_TA_inst.hx  ${ta_name}_TA_inst.h || cp  ${ta_name}_TA_inst.hx  ${ta_name}_TA_inst.h
    DEPENDS ${ta_name}_TA.ccx ${ta_name}_TA_type.hx ${ta_name}_TA_inst.hx
  )
#   ADD_CUSTOM_TARGET(force_ta maketa ${maketa_flags} -autohx ${maketa_includes} ${PROJECT_NAME} ${MAKETA_HEADERS}
# #     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#   )
  SET_TA_PROPS(${ta_name} ${path})
ENDMACRO (CREATE_MAKETA_COMMAND)

