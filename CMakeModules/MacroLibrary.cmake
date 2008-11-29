# this is a library of useful macros for use throughout the build process

# add path to each of the files in the file list, and return result in first arg
# IMPORTANT: surround the _file_list arg in double quotes when calling!

MACRO (ADD_PATH_TO_FILES _result _path _file_list)
   SET(${_result})
   FOREACH(_current ${_file_list})
      SET(${_result} ${${_result}} "${_path}/${_current}")
   ENDFOREACH(_current ${_file_list})
ENDMACRO(ADD_PATH_TO_FILES)

# use this instead of TARGET_LINK_LIBRARIES -- sets suffix and other properties
MACRO (EMERGENT_LINK_LIBRARIES _targ)
    target_link_libraries(${_targ} ${EMERGENT_LIBRARIES})
    set_target_properties(${_targ} PROPERTIES
      VERSION ${EMERGENT_LIB_VERSION}
      SOVERSION ${EMERGENT_LIB_SOVERSION}
    )
ENDMACRO (EMERGENT_LINK_LIBRARIES)

