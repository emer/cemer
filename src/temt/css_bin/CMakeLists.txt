########### css bin target ###############

set(css_SRCS main.cpp)

add_executable(css ${css_SRCS})

target_link_libraries(css temt ${EMERGENT_DEP_LIBRARIES})

########### install files ###############

install(TARGETS css
  RUNTIME DESTINATION bin
)

