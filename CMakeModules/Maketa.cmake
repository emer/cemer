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
  set(MAKETA_FLAGS -css -cpp=\"${CMAKE_CXX_COMPILER} -x c++-header -E\")
  
  macro(SET_TA_PROPS outfile)
    SET_SOURCE_FILES_PROPERTIES(${outfile}
      PROPERTIES COMPILE_FLAGS "-O0 -g0" GENERATED 1    
      )
  endmacro(SET_TA_PROPS)
endif (WIN32)

macro(CREATE_MAKETA_COMMAND infile outfile)
  MAKETA_GET_INC_DIRS(maketa_includes)
  
  add_custom_command(
    OUTPUT ${outfile}
    COMMAND maketa ${MAKETA_FLAGS} ${maketa_includes} -o ${outfile} ${infile}
    DEPENDS ${infile})
  SET_TA_PROPS(${outfile})
endmacro (CREATE_MAKETA_COMMAND)

# new maketa command -- based directly on QT4_WRAP_CPP in Qt4Macros.cmake
# uses QT4 macros directly
macro(MAKETA_WRAP_H outfiles)
  QT4_EXTRACT_OPTIONS(mta_files mta_options ${ARGN})

  foreach (it ${mta_files})
    get_filename_component(it ${it} ABSOLUTE)
    QT4_MAKE_OUTPUT_FILE(${it} TA_ cxx outfile)
    CREATE_MAKETA_COMMAND(${it} ${outfile})
    set(${outfiles} ${${outfiles}} ${outfile})
  endforeach()
endmacro (MAKETA_WRAP_H)

