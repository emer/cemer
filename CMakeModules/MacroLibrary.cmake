# this is a library of useful macros for use throughout the build process

# add path to each of the files in the file list, and return result in first arg
# IMPORTANT: surround the _file_list arg in double quotes when calling!

macro(ADD_PATH_TO_FILES _result _path _file_list)
   SET(${_result})
   FOREACH(_current ${_file_list})
      SET(${_result} ${${_result}} "${_path}/${_current}")
   ENDFOREACH(_current ${_file_list})
endmacro(ADD_PATH_TO_FILES)


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

