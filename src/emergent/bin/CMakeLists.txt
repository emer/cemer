########### emergent bin target ###############

set(emergentbin_SRCS main.cpp)

add_executable(emergent ${emergentbin_SRCS})

target_link_libraries(emergent emergentlib temt ${EMERGENT_DEP_LIBRARIES})

########### install files ###############

install(TARGETS emergent
  RUNTIME DESTINATION bin
)
