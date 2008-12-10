#####################################
##	MAKETA suport for cmake

MACRO (MAKETA_GET_INC_DIRS _MAKETA_INC_DIRS)
  SET(${_MAKETA_INC_DIRS})
  GET_DIRECTORY_PROPERTY(_inc_DIRS INCLUDE_DIRECTORIES)

  FOREACH(_current ${_inc_DIRS})
    SET(${_MAKETA_INC_DIRS} ${${_MAKETA_INC_DIRS}} "-I${_current}")
  ENDFOREACH(_current ${_inc_DIRS})
ENDMACRO(MAKETA_GET_INC_DIRS)

if (WIN32)
  SET(MAKETA_FLAGS -css -hx -win_dll)
  
  # this is critical for allowing dependencies to work out, and for compiling w/out extra load
  MACRO (SET_TA_PROPS _ta_name _path)
    SET_SOURCE_FILES_PROPERTIES(${_path}/${_ta_name}_TA.cpp
      PROPERTIES COMPILE_FLAGS "/Od" GENERATED 1    
      )
  ENDMACRO(SET_TA_PROPS)

else (WIN32)

  # TODO: add a find cpp thing
  SET(MAKETA_FLAGS -hx -css -cpp=\"g++ -E\")
  
  MACRO (SET_TA_PROPS _ta_name _path)
    SET_SOURCE_FILES_PROPERTIES(${_path}/${_ta_name}_TA.cpp
      PROPERTIES COMPILE_FLAGS "-O0 -g0" GENERATED 1    
      )
  ENDMACRO(SET_TA_PROPS)
endif (WIN32)

MACRO (CREATE_MAKETA_COMMAND _ta_name _path _maketa_headers)
  MAKETA_GET_INC_DIRS(maketa_includes)
  
  SET(pta ${_path}/${_ta_name})
  
  ADD_CUSTOM_COMMAND(
    OUTPUT ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
    WORKING_DIRECTORY ${_path}
    COMMAND maketa ${MAKETA_FLAGS} ${maketa_includes} ${_ta_name} ${_maketa_headers}
    DEPENDS ${_maketa_headers}
    )
  # note: don't concat these commands, not tested on Windows
  # note: only use cmake -E commands, must work on Unix and Windows
  ADD_CUSTOM_COMMAND(
    OUTPUT ${pta}_TA.cpp ${pta}_TA_type.h ${pta}_TA_inst.h 
    WORKING_DIRECTORY ${_path}
    COMMAND ${CMAKE_COMMAND} -E compare_files ${pta}_TA.ccx  ${pta}_TA.cpp || ${CMAKE_COMMAND} -E copy  ${pta}_TA.ccx  ${pta}_TA.cpp
    COMMAND ${CMAKE_COMMAND} -E compare_files ${pta}_TA_type.hx  ${pta}_TA_type.h || ${CMAKE_COMMAND} -E copy  ${pta}_TA_type.hx  ${pta}_TA_type.h
    COMMAND ${CMAKE_COMMAND} -E compare_files  ${pta}_TA_inst.hx  ${pta}_TA_inst.h || ${CMAKE_COMMAND} -E copy  ${pta}_TA_inst.hx  ${pta}_TA_inst.h
    DEPENDS ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
    )
  
#nn  ADD_CUSTOM_TARGET(${_ta_name}_TA.cpp DEPENDS ${_maketa_headers})

  # this target is so that other targets can depend on us
#nn  ADD_CUSTOM_TARGET (${_ta_name}_TA
#nn    DEPENDS ${pta}_TA.cpp
#nn  )

  ADD_CUSTOM_TARGET(force_ta_${_ta_name}
    COMMAND maketa ${MAKETA_FLAGS} -autohx ${maketa_includes} ${_ta_name} ${_maketa_headers}
    WORKING_DIRECTORY ${_path}
  )

  ADD_CUSTOM_TARGET(clean_ta_${_ta_name}
    COMMAND rm -f ${pta}_TA.cpp ${pta}_TA_type.h ${pta}_TA_inst.h ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
  )

  SET_TA_PROPS(${_ta_name} ${_path})
ENDMACRO (CREATE_MAKETA_COMMAND)

