# This is a command to clean files

#get_property(cur_dir GLOBAL DIRECTORY)

message(STATUS "Removing TA_*.cxx files in directory: ${CMAKE_CURRENT_BINARY_DIR}")
file(GLOB tafs TA_*.cxx)
if(tafs STREQUAL "")
  message(STATUS "No TA_*.cxx files found -- must be already clean!")
else()
  file(REMOVE ${tafs})
endif()

