########### demo/cs install files ###############

FILE(GLOB PROJ_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proj")

install(FILES ${PROJ_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/demo/cs
)
