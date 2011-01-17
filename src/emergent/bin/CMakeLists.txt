########### emergent bin target ###############

set(emergentbin_SRCS main.cpp)

# Add icon resource for window builds
if (WIN32)
  set(emergentbin_SRCS ${emergentbin_SRCS} icon.rc)
endif (WIN32)

add_executable(emergent ${emergentbin_SRCS})

EMERGENT_LINK_EXECUTABLES(emergent emergentlib temt)

# Windows import/export control
if (WIN32)
  add_definitions(-DTA_DLL -DEMERGENT_DLL)
endif (WIN32)

########### install files ###############

install(TARGETS emergent
  RUNTIME DESTINATION bin
)
