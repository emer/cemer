# this is a library of useful macros for use throughout the build process

# add path to each of the files in the file list, and return result in first arg
# IMPORTANT: surround the _file_list arg in double quotes when calling!

macro(ADD_PATH_TO_FILES _result _path _file_list)
   SET(${_result})
   FOREACH(_current ${_file_list})
      SET(${_result} ${${_result}} "${_path}/${_current}")
   ENDFOREACH(_current ${_file_list})
endmacro(ADD_PATH_TO_FILES)

# copy the _targ (executable or lib) to the _dest under home, ex. bin or lib
# various issues and bugs make this a lot harder than it should be :(
macro(ADD_COPY_TO_LIBRARY_TARGET _targ _dest)
  if (WIN32)
    #note: bug makes LOCATION return non-suffixed name, so we have to use this gruesome workaround
    file(TO_NATIVE_PATH "${CMAKE_HOME_DIRECTORY}" CMAKE_HOME_DIRECTORY_NATIVE)
    set(TARG_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}${_targ}*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    
    add_custom_command(TARGET ${_targ} POST_BUILD
      COMMAND copy /Y ${TARG_FILENAME}
        \"${CMAKE_HOME_DIRECTORY_NATIVE}\\${_dest}\"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}\\${CMAKE_CFG_INTDIR}
    )
  else (WIN32)
    get_target_property(TARG_FILENAME ${_targ} LOCATION)
    add_custom_command(TARGET ${_targ} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${TARG_FILENAME}
        ${CMAKE_HOME_DIRECTORY}/${_dest})
  endif (WIN32)
endmacro (ADD_COPY_TO_LIBRARY_TARGET)

macro(ADD_COPY_TO_EXECUTABLE_TARGET _targ _dest)
  if (WIN32)
    #note: bug makes LOCATION return non-suffixed name, so we have to use this gruesome workaround
    file(TO_NATIVE_PATH "${CMAKE_HOME_DIRECTORY}" CMAKE_HOME_DIRECTORY_NATIVE)
    set(TARG_FILENAME "${_targ}*${CMAKE_EXECUTABLE_SUFFIX}")
    
    add_custom_command(TARGET ${_targ} POST_BUILD
      COMMAND copy /Y ${TARG_FILENAME}
        \"${CMAKE_HOME_DIRECTORY_NATIVE}\\${_dest}\"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}\\${CMAKE_CFG_INTDIR}
      )
  else (WIN32)
    get_target_property(TARG_FILENAME ${_targ} LOCATION)
    add_custom_command(TARGET ${_targ} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${TARG_FILENAME}
        ${CMAKE_HOME_DIRECTORY}/${_dest})
  endif (WIN32)
endmacro (ADD_COPY_TO_EXECUTABLE_TARGET)

# use this instead of TARGET_LINK_LIBRARIES -- sets suffix and other properties, copies target
macro(EMERGENT_LINK_LIBRARIES _targ _xtra_libs)
  #note: we need to use a wildcarded name because a bug makes it return the non_postfix name
  # in every context
  #set(TARG_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}${_targ}*${CMAKE_SHARED_LIBRARY_SUFFIX}")
  target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_DEP_LIBRARIES})
  
  set_target_properties(${_targ} PROPERTIES
    VERSION ${EMERGENT_LIB_VERSION}
    SOVERSION ${EMERGENT_LIB_SOVERSION}
  )
  # copy it up to the shared dir to simplify dev 
  ADD_COPY_TO_LIBRARY_TARGET(${_targ} bin)
endmacro (EMERGENT_LINK_LIBRARIES)

# use this instead of TARGET_LINK_LIBRARIES -- copies target
macro(EMERGENT_LINK_EXECUTABLES _targ _xtra_libs)
  set(TARG_FILENAME "${_targ}${CMAKE_EXECUTABLE_SUFFIX}")
  target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_DEP_LIBRARIES})
  
  # cmake doesn't set the SUFFIX guys, so we do it here, particularly
  # for XCode/VS where *all* contexts are generated at once
  set_target_properties(${_targ} PROPERTIES
    VERSION ${EMERGENT_LIB_VERSION}
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}"
    RELWITHDEBINFO_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}"
    MINSIZEREL_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}"
  )
  # copy it up to the shared dir to simplify dev 
  if (WIN32)
    ADD_COPY_TO_EXECUTABLE_TARGET(${_targ} bin)
  else (WIN32)
    ADD_COPY_TO_EXECUTABLE_TARGET(${_targ} lib)
  endif (WIN32)
endmacro (EMERGENT_LINK_EXECUTABLES)

