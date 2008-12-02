########### css bin target ###############

set(css_SRCS main.cpp)

add_executable(css ${css_SRCS})

target_link_libraries(css temt ${EMERGENT_DEP_LIBRARIES})

if (WIN32)
  add_definitions(-DTA_DLL)
endif (WIN32)

########### install files ###############

install(TARGETS css
  RUNTIME DESTINATION bin
)

