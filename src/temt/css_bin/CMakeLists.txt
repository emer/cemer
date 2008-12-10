########### css bin target ###############

set(css_SRCS main.cpp)

add_executable(css ${css_SRCS})

EMERGENT_LINK_EXECUTABLES(css temt)

# Windows import/export control
if (WIN32)
  add_definitions(-DTA_DLL)
endif (WIN32)

########### install files ###############

install(TARGETS css
  RUNTIME DESTINATION bin
)

