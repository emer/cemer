# this is a library of useful macros for use throughout the build process

# add path to each of the files in the file list, and return result in first arg
# IMPORTANT: surround the _file_list arg in double quotes when calling!

macro(ADD_PATH_TO_FILES _result _path _file_list)
   SET(${_result})
   FOREACH(_current ${_file_list})
      SET(${_result} ${${_result}} "${_path}/${_current}")
   ENDFOREACH(_current ${_file_list})
endmacro(ADD_PATH_TO_FILES)

macro(GET_NO_H_HEADERS _result _path _file_list)
#  message(STATUS "file list: ${_file_list}")
   SET(${_result})
   FOREACH(_current ${_file_list})
     get_filename_component(_fileo ${_current} NAME)
     get_filename_component(_noh ${_fileo} NAME_WE)
     SET(_fpath "${_path}/${_noh}")
     if(EXISTS ${_fpath})
#       message(STATUS "added: ${_fpath}")
       SET(${_result} ${${_result}} "${_fpath}")
     else (EXISTS ${_fpath})
#        message(STATUS "file: ${_fpath} does not exist!")
     endif(EXISTS ${_fpath})
   ENDFOREACH(_current ${_file_list})
endmacro(GET_NO_H_HEADERS)

# IMPORTANT: surround the _file_list arg in double quotes when calling!
macro(EMERGENT_INSTALL_HEADERS _file_list)
  install(FILES ${_file_list} DESTINATION ${EMERGENT_INCLUDE_DEST})
  GET_NO_H_HEADERS(no_hdrs "${PROJECT_SOURCE_DIR}/include" "${_file_list}")
  install(FILES ${no_hdrs} DESTINATION ${EMERGENT_INCLUDE_DEST})
#  message(STATUS "file list: ${no_hdrs}")
endmacro(EMERGENT_INSTALL_HEADERS)

macro(EMERGENT_INSTALL_QUARTER_HEADERS _file_list)
  install(FILES ${_file_list} DESTINATION ${EMERGENT_INCLUDE_DEST}/Quarter)
endmacro(EMERGENT_INSTALL_QUARTER_HEADERS)

# Use these instead of ADD_EXECUTABLE and ADD_LIBRARY, except for maketa.
# These versions take care of the CMAKE_DEPENDENCY_HACK.
macro (EMERGENT_ADD_EXECUTABLE target)
  add_executable(
    ${target}
    ${ARGN}
  )
  set_target_properties(
    ${target} PROPERTIES
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM "CMAKE_DEPENDENCY_HACK(%)=\"%\""
  )
endmacro (EMERGENT_ADD_EXECUTABLE)

macro (EMERGENT_ADD_LIBRARY target)
  add_library(
    ${target}
    ${ARGN}
  )
  set_target_properties(
    ${target} PROPERTIES
    IMPLICIT_DEPENDS_INCLUDE_TRANSFORM "CMAKE_DEPENDENCY_HACK(%)=\"%\""
  )
endmacro (EMERGENT_ADD_LIBRARY)

# use this instead of TARGET_LINK_LIBRARIES -- sets suffix and other properties
macro(EMERGENT_LINK_LIBRARIES _targ _xtra_libs)
  target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_DEP_LIBRARIES})
  
  set_target_properties(${_targ} PROPERTIES
    VERSION ${EMERGENT_LIB_VERSION}
    SOVERSION ${EMERGENT_LIB_SOVERSION}
  )
endmacro (EMERGENT_LINK_LIBRARIES)

# use this instead of TARGET_LINK_EXECUTABLES -- sets properties
macro(EMERGENT_LINK_EXECUTABLES _targ _xtra_libs)
  target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_DEP_LIBRARIES})
  
  # cmake doesn't set the SUFFIX guys, so we do it here, particularly
  # for XCode/VS where *all* contexts are generated at once
  set_target_properties(${_targ} PROPERTIES
  #no!  VERSION ${EMERGENT_LIB_VERSION}
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}"
    RELWITHDEBINFO_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}"
    MINSIZEREL_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}"
  )
endmacro (EMERGENT_LINK_EXECUTABLES)

