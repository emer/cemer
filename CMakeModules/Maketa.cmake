#####################################
##	MAKETA suport for cmake

# this is just a copy of QT4_EXTRACT_OPTIONS -- needed to not conflict with qt5
# from  Qt4Macros.cmake
macro (MAKETA_EXTRACT_OPTIONS _maketa_files _maketa_options)
  set(${_maketa_files})
  set(${_maketa_options})
  set(_MAKETA_DOING_OPTIONS FALSE)
  foreach(_currentArg ${ARGN})
    if ("${_currentArg}" STREQUAL "OPTIONS")
      set(_MAKETA_DOING_OPTIONS TRUE)
    else ()
      if(_MAKETA_DOING_OPTIONS)
        list(APPEND ${_maketa_options} "${_currentArg}")
      else()
        list(APPEND ${_maketa_files} "${_currentArg}")
      endif()
    endif ()
  endforeach()
endmacro ()

# copy of QT4_MAKE_OUTPUT_FILE from Qt4Macros.cmake
macro (MAKETA_MAKE_OUTPUT_FILE infile prefix ext outfile )
  string(LENGTH ${CMAKE_CURRENT_BINARY_DIR} _binlength)
  string(LENGTH ${infile} _infileLength)
  set(_checkinfile ${CMAKE_CURRENT_SOURCE_DIR})
  if(_infileLength GREATER _binlength)
    string(SUBSTRING "${infile}" 0 ${_binlength} _checkinfile)
    if(_checkinfile STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
      file(RELATIVE_PATH rel ${CMAKE_CURRENT_BINARY_DIR} ${infile})
    else()
      file(RELATIVE_PATH rel ${CMAKE_CURRENT_SOURCE_DIR} ${infile})
    endif()
  else()
    file(RELATIVE_PATH rel ${CMAKE_CURRENT_SOURCE_DIR} ${infile})
  endif()
  if(WIN32 AND rel MATCHES "^[a-zA-Z]:") # absolute path
    string(REGEX REPLACE "^([a-zA-Z]):(.*)$" "\\1_\\2" rel "${rel}")
  endif()
  set(_outfile "${CMAKE_CURRENT_BINARY_DIR}/${rel}")
  string(REPLACE ".." "__" _outfile ${_outfile})
  get_filename_component(outpath ${_outfile} PATH)
  get_filename_component(_outfile ${_outfile} NAME_WE)
  file(MAKE_DIRECTORY ${outpath})
  set(${outfile} ${outpath}/${prefix}${_outfile}.${ext})
endmacro ()


macro (MAKETA_GET_INC_DIRS _MAKETA_INC_DIRS)
  set(${_MAKETA_INC_DIRS})
  get_directory_property(_inc_DIRS INCLUDE_DIRECTORIES)

  foreach(_current ${_inc_DIRS})
    set(${_MAKETA_INC_DIRS} ${${_MAKETA_INC_DIRS}} "-I${_current}")
  endforeach(_current ${_inc_DIRS})
endmacro(MAKETA_GET_INC_DIRS)

if (WIN32)
  set(MAKETA_FLAGS -css -win_dll)
  
  # this is critical for allowing dependencies to work out, and for compiling w/out extra load
  macro(SET_TA_PROPS outfile)
    SET_SOURCE_FILES_PROPERTIES(${outfile}
      PROPERTIES
        COMPILE_FLAGS "/Od"  # causes warning, but without, compilation is horribly slow
        GENERATED 1
      )
  endmacro(SET_TA_PROPS)

else (WIN32)
  set(MAKETA_CPP "${CMAKE_CXX_COMPILER}" -x c++-header -E)
# todo: can't get it to work with CMAKE_CXX_FLAGS
#  set(MAKETA_CPP "${CMAKE_CXX_COMPILER}" "${CMAKE_CXX_FLAGS}" -x c++-header -E)
#  message(STATUS "MAKETA_CPP ${MAKETA_CPP}")

  set(MAKETA_FLAGS -css -cpp=\"${MAKETA_CPP}\")
  
  macro(SET_TA_PROPS outfile)
    SET_SOURCE_FILES_PROPERTIES(${outfile}
      PROPERTIES COMPILE_FLAGS "-O0 -g0" 
      GENERATED 1
      )
  endmacro(SET_TA_PROPS)
endif (WIN32)

if (MAKETA_CMD)
  message(STATUS "using MAKETA_CMD = ${MAKETA_CMD}")
else (MAKETA_CMD)
  message(STATUS "MAKETA_CMD ${MAKETA_CMD} not set -- using maketa")
  set(MAKETA_CMD "maketa")
endif (MAKETA_CMD)

macro(CREATE_MAKETA_COMMAND infile outfile)
  MAKETA_GET_INC_DIRS(maketa_includes)
  
  add_custom_command(
    OUTPUT ${outfile}
    COMMAND ${MAKETA_CMD} ${MAKETA_FLAGS} ${maketa_includes} -o ${outfile} ${infile}
    DEPENDS ${infile}
  )
  # IMPORTANT: setting these props triggers a full rebuild of the source whenever
  # the list of files changes -- so we cannot do this -- now with distributed maketa
  # this is not such a big deal, so we let it go..
  # SET_TA_PROPS(${outfile})
endmacro (CREATE_MAKETA_COMMAND)

# new maketa command -- based directly on QT4_WRAP_CPP in Qt4Macros.cmake
# MUST call with OPTIONS and the tag name for the clean_ta_xx command
macro(MAKETA_WRAP_H outfiles)
  MAKETA_EXTRACT_OPTIONS(mta_files mta_options ${ARGN})
  if(mta_options STREQUAL "")
    message(ERROR "MAKETA_WRAP_H must be called with OPTIONS xxx where xxx is code name for make clean target (clean_ta_xxx)")
  endif()
  foreach (it ${mta_files})
    get_filename_component(it ${it} ABSOLUTE)
    
    MAKETA_MAKE_OUTPUT_FILE(${it} TA_ cxx outfile)
    CREATE_MAKETA_COMMAND(${it} ${outfile})
    set(${outfiles} ${${outfiles}} ${outfile})
  endforeach()

  list(GET ${outfiles} 0 firstfile)
  get_filename_component(outpath ${firstfile} PATH)
  add_custom_target(clean_ta_${mta_options}
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/CMakeModules/maketa_clean.cmake WORKING_DIRECTORY ${outpath}
  )

endmacro (MAKETA_WRAP_H)

# this version has an extra dependency for files with multiple inheritance (MI)
macro(CREATE_MAKETA_COMMAND_MI infile outfile extradep)
  MAKETA_GET_INC_DIRS(maketa_includes)

  set(exdeplist ${extradep})

  # we're just going to have to assume it is 2 dependencies!
  list(GET exdeplist 0 dep0)
  list(GET exdeplist 1 dep1)

  # foreach(dep ${exdeplist})
  #   message(STATUS "dep: ${dep}")
  #   set(deps "${deps} ${dep}")
  # endforeach()

  add_custom_command(
    OUTPUT ${outfile}
    COMMAND ${MAKETA_CMD} ${MAKETA_FLAGS} ${maketa_includes} -o ${outfile} ${infile}
    DEPENDS ${infile} ${dep0} ${dep1}
  )
endmacro (CREATE_MAKETA_COMMAND_MI)

macro(MAKETA_WRAP_H_MI rval outstr depstr)
  # this is key for turning the string args back into lists!!
  set(outfiles ${outstr})
  set(extradeps ${depstr})

  list(LENGTH outfiles len1)
  list(LENGTH extradeps lend)
  math(EXPR len2 "${len1} - 1")

  math(EXPR dper "${lend} / ${len1}")
  math(EXPR dperm1 "${dper} - 1")

  set(didx 0)
  foreach(val RANGE ${len2})
    list(GET outfiles ${val} it)

    set(deps "")

    foreach(dper RANGE ${dperm1})
      list(GET extradeps ${didx} edep)
      set(deps ${deps} ${edep})
      math(EXPR didx "${didx} + 1")
    endforeach()

    message(STATUS "${val} file: ${it}\ndepends on: ${deps}")

    get_filename_component(it ${it} ABSOLUTE)
    
    MAKETA_MAKE_OUTPUT_FILE(${it} TA_ cxx outfile)
    CREATE_MAKETA_COMMAND_MI(${it} ${outfile} "${deps}")
    set(${rval} ${${rval}} ${outfile})
  endforeach()

  # list(GET ${outfiles} 0 firstfile)
  # get_filename_component(outpath ${firstfile} PATH)
  # add_custom_target(clean_ta_${mta_options}
  #   COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/CMakeModules/maketa_clean.cmake WORKING_DIRECTORY ${outpath})

endmacro (MAKETA_WRAP_H_MI)

