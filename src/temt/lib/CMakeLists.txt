########### temt lib target ###############

# this is where all the temt code gets built into a single library
# we read in the list of files from the relevant directories and then 
# add relative path for here, then just list them all as compile targets
# also, all moc generation happens here, but all maket generation must
# happen in the original source directory!!  see ta for example

# cmake 2.4 needs absolute pathing to find _TA.cpp file
#set(mypath "..")
set(mypath ${PROJECT_SOURCE_DIR}/src/temt)

# first: include all the files and add the full path

# ta_core
include(${PROJECT_SOURCE_DIR}/src/temt/ta_core/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_core_SRCS ${mypath}/ta_core "${ta_core_SRCS}")
ADD_PATH_TO_FILES(temt_ta_core_HEADS_MOC ${mypath}/ta_core "${ta_core_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_ta_core_HEADS_MAKETA ${mypath}/ta_core "${ta_core_HEADS_MAKETA}")
ADD_PATH_TO_FILES(temt_ta_core_HEADS ${mypath}/ta_core "${ta_core_HEADS}")

ADD_PATH_TO_FILES(temt_ta_core_HEADS_MAKETA_MI ${mypath}/ta_core "${ta_core_HEADS_MAKETA_MI}")
ADD_PATH_TO_FILES(temt_ta_core_HEADS_MAKETA_MI_DEPS ${mypath}/ta_core "${ta_core_HEADS_MAKETA_MI_DEPS}")

# ta_math
include(${PROJECT_SOURCE_DIR}/src/temt/ta_math/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_math_SRCS ${mypath}/ta_math "${ta_math_SRCS}")
ADD_PATH_TO_FILES(temt_ta_math_HEADS_MAKETA ${mypath}/ta_math "${ta_math_HEADS_MAKETA}")
ADD_PATH_TO_FILES(temt_ta_math_HEADS ${mypath}/ta_math "${ta_math_HEADS}")

# ta_program
include(${PROJECT_SOURCE_DIR}/src/temt/ta_program/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_program_SRCS ${mypath}/ta_program "${ta_program_SRCS}")
ADD_PATH_TO_FILES(temt_ta_program_HEADS_MAKETA ${mypath}/ta_program "${ta_program_HEADS_MAKETA}")
ADD_PATH_TO_FILES(temt_ta_program_HEADS ${mypath}/ta_program "${ta_program_HEADS}")

ADD_PATH_TO_FILES(temt_ta_program_HEADS_MAKETA_MI ${mypath}/ta_program "${ta_program_HEADS_MAKETA_MI}")
ADD_PATH_TO_FILES(temt_ta_program_HEADS_MAKETA_MI_DEPS ${mypath}/ta_program "${ta_program_HEADS_MAKETA_MI_DEPS}")

# ta_data
include(${PROJECT_SOURCE_DIR}/src/temt/ta_data/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_data_SRCS ${mypath}/ta_data "${ta_data_SRCS}")
ADD_PATH_TO_FILES(temt_ta_data_HEADS_MAKETA ${mypath}/ta_data "${ta_data_HEADS_MAKETA}")
ADD_PATH_TO_FILES(temt_ta_data_HEADS ${mypath}/ta_data "${ta_data_HEADS}")

# ta_proj
include(${PROJECT_SOURCE_DIR}/src/temt/ta_proj/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_proj_SRCS ${mypath}/ta_proj "${ta_proj_SRCS}")
ADD_PATH_TO_FILES(temt_ta_proj_HEADS_MOC ${mypath}/ta_proj "${ta_proj_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_ta_proj_HEADS_MAKETA ${mypath}/ta_proj "${ta_proj_HEADS_MAKETA}")
ADD_PATH_TO_FILES(temt_ta_proj_HEADS ${mypath}/ta_proj "${ta_proj_HEADS}")

ADD_PATH_TO_FILES(temt_ta_proj_HEADS_MAKETA_MI ${mypath}/ta_proj "${ta_proj_HEADS_MAKETA_MI}")
ADD_PATH_TO_FILES(temt_ta_proj_HEADS_MAKETA_MI_DEPS ${mypath}/ta_proj "${ta_proj_HEADS_MAKETA_MI_DEPS}")

# ta_gui
include(${PROJECT_SOURCE_DIR}/src/temt/ta_gui/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_gui_SRCS ${mypath}/ta_gui "${ta_gui_SRCS}")
ADD_PATH_TO_FILES(temt_ta_gui_HEADS_MOC ${mypath}/ta_gui "${ta_gui_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_ta_gui_HEADS_MAKETA ${mypath}/ta_gui "${ta_gui_HEADS_MAKETA}")
ADD_PATH_TO_FILES(temt_ta_gui_HEADS ${mypath}/ta_gui "${ta_gui_HEADS}")
ADD_PATH_TO_FILES(temt_ta_gui_SRCS_UI ${mypath}/ta_gui "${ta_gui_SRCS_UI}")

ADD_PATH_TO_FILES(temt_ta_gui_HEADS_MAKETA_MI ${mypath}/ta_gui "${ta_gui_HEADS_MAKETA_MI}")
ADD_PATH_TO_FILES(temt_ta_gui_HEADS_MAKETA_MI_DEPS ${mypath}/ta_gui "${ta_gui_HEADS_MAKETA_MI_DEPS}")

ADD_PATH_TO_FILES(temt_ta_gui_QRC ${mypath}/ta_gui "${ta_gui_QRC}")

# ta_3d
include(${PROJECT_SOURCE_DIR}/src/temt/ta_3d/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_ta_3d_SRCS ${mypath}/ta_3d "${ta_3d_SRCS}")
ADD_PATH_TO_FILES(temt_ta_3d_HEADS_MOC ${mypath}/ta_3d "${ta_3d_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_ta_3d_HEADS_MAKETA ${mypath}/ta_3d "${ta_3d_HEADS_MAKETA}")
ADD_PATH_TO_FILES(temt_ta_3d_HEADS ${mypath}/ta_3d "${ta_3d_HEADS}")

ADD_PATH_TO_FILES(temt_ta_3d_HEADS_MAKETA_MI ${mypath}/ta_3d "${ta_3d_HEADS_MAKETA_MI}")
ADD_PATH_TO_FILES(temt_ta_3d_HEADS_MAKETA_MI_DEPS ${mypath}/ta_3d "${ta_3d_HEADS_MAKETA_MI_DEPS}")

ADD_PATH_TO_FILES(temt_ta_3d_QRC ${mypath}/ta_3d "${ta_3d_QRC}")

# css
include(${PROJECT_SOURCE_DIR}/src/temt/css/CMakeFiles.txt)
ADD_PATH_TO_FILES(temt_css_SRCS ${mypath}/css "${css_SRCS}")
ADD_PATH_TO_FILES(temt_css_HEADS_MOC ${mypath}/css "${css_HEADS_MOC}")
ADD_PATH_TO_FILES(temt_css_HEADS ${mypath}/css "${css_HEADS}")

# next: use the wrap code to get maketa and moc to run:

MAKETA_WRAP_H(temt_ta_core_SRCS_MAKETA ${temt_ta_core_HEADS_MAKETA} OPTIONS ta_core)
MAKETA_WRAP_H(temt_ta_math_SRCS_MAKETA ${temt_ta_math_HEADS_MAKETA} OPTIONS ta_math)
MAKETA_WRAP_H(temt_ta_program_SRCS_MAKETA ${temt_ta_program_HEADS_MAKETA} OPTIONS ta_program)
MAKETA_WRAP_H(temt_ta_data_SRCS_MAKETA ${temt_ta_data_HEADS_MAKETA} OPTIONS ta_data)
MAKETA_WRAP_H(temt_ta_proj_SRCS_MAKETA ${temt_ta_proj_HEADS_MAKETA} OPTIONS ta_proj)
MAKETA_WRAP_H(temt_ta_gui_SRCS_MAKETA ${temt_ta_gui_HEADS_MAKETA} OPTIONS ta_gui)
MAKETA_WRAP_H(temt_ta_3d_SRCS_MAKETA ${temt_ta_3d_HEADS_MAKETA} OPTIONS ta_3d)

# include MAKETA_MI guys:
MAKETA_WRAP_H_MI(temt_ta_core_SRCS_MAKETA_MI "${temt_ta_core_HEADS_MAKETA_MI}" "${temt_ta_core_HEADS_MAKETA_MI_DEPS}")
MAKETA_WRAP_H_MI(temt_ta_program_SRCS_MAKETA_MI "${temt_ta_program_HEADS_MAKETA_MI}" "${temt_ta_program_HEADS_MAKETA_MI_DEPS}")
MAKETA_WRAP_H_MI(temt_ta_proj_SRCS_MAKETA_MI "${temt_ta_proj_HEADS_MAKETA_MI}" "${temt_ta_proj_HEADS_MAKETA_MI_DEPS}")
MAKETA_WRAP_H_MI(temt_ta_gui_SRCS_MAKETA_MI "${temt_ta_gui_HEADS_MAKETA_MI}" "${temt_ta_gui_HEADS_MAKETA_MI_DEPS}")
MAKETA_WRAP_H_MI(temt_ta_3d_SRCS_MAKETA_MI "${temt_ta_3d_HEADS_MAKETA_MI}" "${temt_ta_3d_HEADS_MAKETA_MI_DEPS}")

if (QT_USE_5)
  qt5_wrap_ui(temt_ta_gui_SRCS_UIC ${temt_ta_gui_SRCS_UI})
  qt5_wrap_cpp(temt_ta_core_SRCS_MOC ${temt_ta_core_HEADS_MOC})
  # math, program, data don't have MOC files
  qt5_wrap_cpp(temt_ta_proj_SRCS_MOC ${temt_ta_proj_HEADS_MOC})
  qt5_wrap_cpp(temt_ta_gui_SRCS_MOC ${temt_ta_gui_HEADS_MOC})
  qt5_wrap_cpp(temt_ta_3d_SRCS_MOC ${temt_ta_3d_HEADS_MOC})
  qt5_wrap_cpp(temt_css_SRCS_MOC ${temt_css_HEADS_MOC})

  qt5_add_resources(temt_ta_gui_SRCS_QRC ${temt_ta_gui_QRC})
  qt5_add_resources(temt_ta_3d_SRCS_QRC ${temt_ta_3d_QRC})
  
else (QT_USE_5)
  QT4_WRAP_UI(temt_ta_gui_SRCS_UIC ${temt_ta_gui_SRCS_UI})
  # math, program, data don't have MOC files
  QT4_WRAP_CPP(temt_ta_core_SRCS_MOC ${temt_ta_core_HEADS_MOC})
  QT4_WRAP_CPP(temt_ta_proj_SRCS_MOC ${temt_ta_proj_HEADS_MOC})
  QT4_WRAP_CPP(temt_ta_gui_SRCS_MOC ${temt_ta_gui_HEADS_MOC})
  QT4_WRAP_CPP(temt_ta_3d_SRCS_MOC ${temt_ta_3d_HEADS_MOC})
  QT4_WRAP_CPP(temt_css_SRCS_MOC ${temt_css_HEADS_MOC})
endif (QT_USE_5)


# just list whole set of files here -- and also include the headers for IDE's
set(temtlib_SRCS
  ${temt_ta_core_SRCS} ${temt_ta_core_SRCS_MAKETA} ${temt_ta_core_SRCS_MAKETA_MI} ${temt_ta_core_SRCS_MOC}
  ${temt_ta_math_SRCS} ${temt_ta_math_SRCS_MAKETA} ${temt_ta_math_SRCS_MAKETA_MI} ${temt_ta_math_SRCS_MOC}
  ${temt_ta_program_SRCS} ${temt_ta_program_SRCS_MAKETA} ${temt_ta_program_SRCS_MAKETA_MI} ${temt_ta_program_SRCS_MOC}
  ${temt_ta_data_SRCS} ${temt_ta_data_SRCS_MAKETA} ${temt_ta_data_SRCS_MAKETA_MI} ${temt_ta_data_SRCS_MOC}
  ${temt_ta_proj_SRCS} ${temt_ta_proj_SRCS_MAKETA} ${temt_ta_proj_SRCS_MAKETA_MI} ${temt_ta_proj_SRCS_MOC}
  ${temt_ta_gui_SRCS} ${temt_ta_gui_SRCS_MAKETA} ${temt_ta_gui_SRCS_MAKETA_MI} ${temt_ta_gui_SRCS_MOC}
  ${temt_ta_3d_SRCS} ${temt_ta_3d_SRCS_MAKETA} ${temt_ta_3d_SRCS_MAKETA_MI} ${temt_ta_3d_SRCS_MOC}

  ${temt_ta_gui_SRCS_UIC}
  ${temt_css_SRCS}
  ${temt_css_SRCS_MOC}

  ${temt_ta_core_HEADS}
  ${temt_ta_math_HEADS}
  ${temt_ta_program_HEADS}
  ${temt_ta_data_HEADS}
  ${temt_ta_proj_HEADS}
  ${temt_ta_gui_HEADS}
  ${temt_ta_3d_HEADS}
  ${temt_css_HEADS}

  ${temt_ta_gui_SRCS_QRC}
  ${temt_ta_3d_SRCS_QRC}
)

EMERGENT_ADD_LIBRARY(temt SHARED ${temtlib_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/svnrev.h)
if (WIN32)
  set_target_properties(temt PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif (WIN32)

########### get svn revision ###############
# http://stackoverflow.com/questions/3780667/use-cmake-to-get-build-time-svn-revision
#
# include the output directory, where the svnrev.h file is generated
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# providing a filename that will never exist ensures this target
# is built every time
add_custom_target(svn_revision_header ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doesnotexist.h)

# this purposefully does not create the doesnotexist.h file!
# creates svnrev.h using cmake script
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doesnotexist.h
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR="${PROJECT_SOURCE_DIR}"
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/GetSvn.cmake)

# svnrev.h is a generated file
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/svnrev.h
    PROPERTIES GENERATED TRUE
    HEADER_FILE_ONLY TRUE)

# make the library depend on the svn_revision_header
add_dependencies(temt svn_revision_header)
#
########### end get svn revision ###############


# 2nd arg is extra libs to link to
EMERGENT_LINK_LIBRARIES(temt "")

# Windows import/export control
if (WIN32)
  add_definitions(-DTA_DLL -DTA_EXPORTS)
endif (WIN32)

# Defining nominmax will remove all ambiguities regarding the windows.h macros,
# not just the one in qdatetime.h
if (WIN32 AND QT_USE_5)
  add_definitions(-DNOMINMAX)
endif (WIN32 AND QT_USE_5)

########### install files ###############

if (WIN32)
  install(TARGETS temt
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
  )
else (WIN32)
  install(TARGETS temt
    LIBRARY DESTINATION lib
  )
endif (WIN32)
