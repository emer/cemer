#####################################
##	MAKETA suport for cmake

#SET(MAKETA_FLAGS -hx -css -cpp="g++\ -E")
SET(MAKETA_FLAGS -hx -css)
SET(MAKETA_BASE ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
SET(MAKETA_OUT ${MAKETA_BASE}_TA.ccx ${MAKETA_BASE}_TA_type.hx ${MAKETA_BASE}_TA_inst.hx)

# this is a problem: won't let me set this during make file 
# construction without it first existing..
SET_SOURCE_FILES_PROPERTIES(${MAKETA_BASE}_TA.cpp
   PROPERTIES COMPILE_FLAGS -O0
)

MACRO (MAKETA_GET_INC_DIRS _MAKETA_INC_DIRS)
   SET(${_MAKETA_INC_DIRS})
   GET_DIRECTORY_PROPERTY(_inc_DIRS INCLUDE_DIRECTORIES)

   FOREACH(_current ${_inc_DIRS})
      SET(${_MAKETA_INC_DIRS} ${${_MAKETA_INC_DIRS}} "-I${_current}")
   ENDFOREACH(_current ${_inc_DIRS})
ENDMACRO(MAKETA_GET_INC_DIRS)

MAKETA_GET_INC_DIRS(maketa_includes)

ADD_CUSTOM_COMMAND(
   OUTPUT ${MAKETA_OUT}
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
   COMMAND maketa ${MAKETA_FLAGS} ${maketa_includes} ${PROJECT_NAME} ${MAKETA_HEADERS}
   DEPENDS ${MAKETA_HEADERS}
   )

ADD_CUSTOM_COMMAND(
   OUTPUT ${MAKETA_BASE}_TA.cpp ${MAKETA_BASE}_TA_type.h ${MAKETA_BASE}_TA_inst.h 
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
   COMMAND cmp ${MAKETA_BASE}_TA.ccx  ${MAKETA_BASE}_TA.cpp || cp  ${MAKETA_BASE}_TA.ccx  ${MAKETA_BASE}_TA.cpp\; cmp ${MAKETA_BASE}_TA_type.hx  ${MAKETA_BASE}_TA_type.h || cp  ${MAKETA_BASE}_TA_type.hx  ${MAKETA_BASE}_TA_type.h\; cmp ${MAKETA_BASE}_TA_inst.hx  ${MAKETA_BASE}_TA_inst.h || cp  ${MAKETA_BASE}_TA_inst.hx  ${MAKETA_BASE}_TA_inst.h
   DEPENDS ${MAKETA_OUT}
   )

ADD_CUSTOM_TARGET(force_ta ${MAKETA_EXECUTABLE} ${MAKETA_FLAGS} -autohx ${maketa_includes} ${PROJECT_NAME} ${MAKETA_HEADERS}
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
