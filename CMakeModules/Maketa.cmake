#####################################
##	MAKETA suport for cmake

macro (MAKETA_GET_INC_DIRS _MAKETA_INC_DIRS)
  set(${_MAKETA_INC_DIRS})
  get_directory_property(_inc_DIRS INCLUDE_DIRECTORIES)

  foreach(_current ${_inc_DIRS})
    set(${_MAKETA_INC_DIRS} ${${_MAKETA_INC_DIRS}} "-I${_current}")
  endforeach(_current ${_inc_DIRS})
endmacro(MAKETA_GET_INC_DIRS)

if (WIN32)
  set(MAKETA_FLAGS -css -hx -win_dll)
  
  # this is critical for allowing dependencies to work out, and for compiling w/out extra load
  macro(SET_TA_PROPS _ta_name _path)
    SET_SOURCE_FILES_PROPERTIES(${_path}/${_ta_name}_TA.cpp
      PROPERTIES COMPILE_FLAGS "/Od" GENERATED 1    
      )
  endmacro(SET_TA_PROPS)

else (WIN32)

  # TODO: add a find cpp thing
  set(MAKETA_FLAGS -autohx -css -cpp=\"g++ -E\")
  
  macro(SET_TA_PROPS _ta_name _path)
    SET_SOURCE_FILES_PROPERTIES(${_path}/${_ta_name}_TA.cpp
      PROPERTIES COMPILE_FLAGS "-O0 -g0" GENERATED 1    
      )
  endmacro(SET_TA_PROPS)
endif (WIN32)

macro(CREATE_MAKETA_COMMAND _ta_name _path _maketa_headers)
  MAKETA_GET_INC_DIRS(maketa_includes)
  
  set(pta ${_path}/${_ta_name})
  
  add_custom_command(
    OUTPUT ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
    WORKING_DIRECTORY ${_path}
    COMMAND maketa ${MAKETA_FLAGS} ${maketa_includes} ${_ta_name} ${_maketa_headers}
    DEPENDS ${_maketa_headers}
    )
  
  add_custom_target(${_ta_name}_dummy ALL
    DEPENDS ${_ta_name}_TA.ccx ${_ta_name}_TA_type.hx ${_ta_name}_TA_inst.hx
   )
  set_source_files_properties(${_ta_name}_TA.cpp ${_ta_name}_TA_type.h ${_ta_name}_TA_inst.h
    PROPERTIES GENERATED 1
  )

  add_custom_target(force_ta_${_ta_name}
    COMMAND maketa ${MAKETA_FLAGS} -autohx ${maketa_includes} ${_ta_name} ${_maketa_headers}
    WORKING_DIRECTORY ${_path}
  )
if (WIN32)
#NOTE: for some damn reason, clean_ta_xx is included in ALL in VS, even though force_ta_xx is not
# the reason is unknown; tried swapping order here, that didn't change anything
else (WIN32)
  add_custom_target(clean_ta_${_ta_name}
    COMMAND ${CMAKE_COMMAND} -E remove -f ${pta}_TA.cpp ${pta}_TA_type.h ${pta}_TA_inst.h ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
    WORKING_DIRECTORY ${_path}
  )
endif (WIN32)
  SET_TA_PROPS(${_ta_name} ${_path})
endmacro (CREATE_MAKETA_COMMAND)



if (false)
#NOTE: all below will be nuked once confirmed is working on all
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
  
  ADD_CUSTOM_TARGET(force_ta_${_ta_name}
    COMMAND maketa ${MAKETA_FLAGS} -autohx ${maketa_includes} ${_ta_name} ${_maketa_headers}
    WORKING_DIRECTORY ${_path}
  )
if (WIN32)
#NOTE: for some damn reason, clean_ta_xx is included in ALL in VS, even though force_ta_xx is not
# the reason is unknown; tried swapping order here, that didn't change anything
else (WIN32)
  ADD_CUSTOM_TARGET(clean_ta_${_ta_name}
    COMMAND ${CMAKE_COMMAND} -E remove -f ${pta}_TA.cpp ${pta}_TA_type.h ${pta}_TA_inst.h ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
    WORKING_DIRECTORY ${_path}
  )
endif (WIN32)
  SET_TA_PROPS(${_ta_name} ${_path})
ENDMACRO (CREATE_MAKETA_COMMAND)
endif (false)
