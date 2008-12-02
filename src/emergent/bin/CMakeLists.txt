########### emergent bin target ###############

set(emergentbin_SRCS main.cpp)

add_executable(emergent ${emergentbin_SRCS})

target_link_libraries(emergent emergentlib temt ${EMERGENT_DEP_LIBRARIES})

# Windows import/export control
if (WIN32)
  add_definitions(-DTA_DLL -DEMERGENT_DLL)
endif (WIN32)

########### install files ###############

install(TARGETS emergent
  RUNTIME DESTINATION bin
)
