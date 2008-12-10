# this is a library of useful macros for use throughout the build process

# add path to each of the files in the file list, and return result in first arg
# IMPORTANT: surround the _file_list arg in double quotes when calling!

macro(ADD_PATH_TO_FILES _result _path _file_list)
   SET(${_result})
   FOREACH(_current ${_file_list})
      SET(${_result} ${${_result}} "${_path}/${_current}")
   ENDFOREACH(_current ${_file_list})
endmacro(ADD_PATH_TO_FILES)

# use this instead of TARGET_LINK_LIBRARIES -- sets suffix and other properties, copies target
macro(EMERGENT_LINK_LIBRARIES _targ _xtra_libs)
  set(TARG_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}${_targ}${CMAKE_SHARED_LIBRARY_SUFFIX}")
  target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_DEP_LIBRARIES})
  
  set_target_properties(${_targ} PROPERTIES
    VERSION ${EMERGENT_LIB_VERSION}
    SOVERSION ${EMERGENT_LIB_SOVERSION}
  )
  # copy it up to the shared dir to simplify dev 
  if (WIN32)
    add_custom_command(TARGET ${_targ} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${TARG_FILENAME}
        ${CMAKE_HOME_DIRECTORY}/bin)
  else (WIN32)
    add_custom_command(TARGET ${_targ} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${TARG_FILENAME}
        ${CMAKE_HOME_DIRECTORY}/lib)
  endif (WIN32)
endmacro (EMERGENT_LINK_LIBRARIES)

# use this instead of TARGET_LINK_LIBRARIES -- copies target
macro(EMERGENT_LINK_EXECUTABLES _targ _xtra_libs)
  set(TARG_FILENAME "${_targ}${CMAKE_EXECUTABLE_SUFFIX}")
  target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_DEP_LIBRARIES})
  
  # copy it up to the shared dir to simplify dev 
  add_custom_command(TARGET ${_targ} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${TARG_FILENAME}
      ${CMAKE_HOME_DIRECTORY}/bin)
endmacro (EMERGENT_LINK_EXECUTABLES)

