########### css_lib install files ###############

FILE(GLOB CSS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.css")

install(FILES ${CSS_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/css_lib
)

