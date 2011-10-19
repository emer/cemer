########### proj_templates install files ###############

FILE(GLOB PROJ_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proj")
FILE(GLOB PROJ_INFO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.tmplt")

install(FILES ${PROJ_FILES} ${PROJ_INFO_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/proj_templates
)
