# This file is included near the top of both Emergent
# and plugins so that we have a consistent set of 
# default values and processing
#
# Here are the variables that must be set so far:
# (none yet)

# some standard env variables, loaded locally 
# and fixed up for Windows
# note: EMERGENTDIR is really only used on Windows or for development
file(TO_CMAKE_PATH "$ENV{EMERGENTDIR}" EMERGENTDIR)
file(TO_CMAKE_PATH "$ENV{COINDIR}" COINDIR)
file(TO_CMAKE_PATH "$ENV{EMER_MISC_LIBS_DIR}" EMER_MISC_LIBS_DIR)
file(TO_CMAKE_PATH "$ENV{EMER_SVN_LIBS_DIR}" EMER_SVN_LIBS_DIR)
#TODO: set COINDIR according to defaults
#allowed overrides:
file(TO_CMAKE_PATH "$ENV{EMERGENT_PLUGIN_DIR}" EMERGENT_PLUGIN_DIR)

# default build type is RelWithDebInfo
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

# set cache parameter for mpi option
set(MPI_BUILD FALSE CACHE BOOL "Set to true to enable MPI distributed memory system")
# set cache parameter for cuda option
set(CUDA_BUILD FALSE CACHE BOOL "Set to true to enable NVIDIA CUDA GPU compile")

# basic compiler warnings, etc.
# note: really should be testing MSVC but contrary to docs, it is not true using MSVC
if (WIN32)
  # shut off warnings: 4250 (inheritance dominance)
  # 4258;4661;4996 (unsafe crt routines)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4250 /wd4258 /wd4661 /wd4996 /wd4302 /wd4311")

  # enable multi-threaded compiling (always safe, will ignore and print warning when incompatible)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  # need to set this so larger .cpp files don't error (what a stupid non-default!!!)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

  # for the visual studio build, it specifically doesn't set this flag (b/c it is
  # multi-configuration supporting), so we need to set it manually apparently..
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DEBUG")
  endif ()
  
else (WIN32) # assume gcc!!!
  # A function with a non-void return-type that doesn't return a value s/b an error!!!
  # GCC added support for treating this as an error somewhere around version 4.2
  include(CheckCXXCompilerFlag)
  set(GCC_RETURN_TYPE_ERROR "-Werror=return-type")
  set(GCC_RETURN_TYPE_WARN "-Wreturn-type")
  check_cxx_compiler_flag("${GCC_RETURN_TYPE_ERROR}" HAS_RETURN_TYPE_ERROR_FLAG)
  if (HAS_RETURN_TYPE_ERROR_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_RETURN_TYPE_ERROR}")
  else (HAS_RETURN_TYPE_ERROR_FLAG)
    message("Compiler does not support ${GCC_RETURN_TYPE_ERROR}, will use ${GCC_RETURN_TYPE_WARN} instead.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_RETURN_TYPE_WARN}") # at least get a warning!
  endif (HAS_RETURN_TYPE_ERROR_FLAG)

  set(GCC_UNDEF_INLINE "-Wno-undefined-inline -Wno-unknown-warning-option")
#  check_cxx_compiler_flag("${GCC_UNDEF_INLINE}" HAS_UNDEF_INLINE_FLAG)
  set(HAS_UNDEF_INLINE_FLAG TRUE)
  if (HAS_UNDEF_INLINE_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_UNDEF_INLINE}")
  endif (HAS_UNDEF_INLINE_FLAG)
  
  # NOTE:  -ftree-vectorizer-verbose=1 can be interesting but not worth it for a default param -- users may add at own discretion 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wno-unused-variable -Wno-unknown-pragmas")

  # only check shadow on apple -- otherwise older versions of gcc can give excessive
  # errs due to looking at methods in addition to members
  if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
  endif (APPLE)
  
  if(CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    # note: was using  -ffast-math here but this prevents isnan from working, and we depend on that now..
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
  else(CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    # debug mode
    # on Mac/Linux, DEBUG is not defined!
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND SANITIZE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND SANITIZE)
  endif (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")

  if(NOT NOT_NATIVE MATCHES "not-native")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  endif (NOT NOT_NATIVE MATCHES "not-native")

  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # -Wunsequenced is causing seemingly errouneous warnings for TA files
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unsequenced")
  else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fabi-version=6 ")
  endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

#    message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
#    add the automatically determined parts of the RPATH
#    which point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

endif (WIN32)

