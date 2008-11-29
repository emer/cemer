########### demo/virt_env install files ###############

FILE(GLOB PROJ_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proj")
FILE(GLOB IV_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.iv")
FILE(GLOB PNG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

install(FILES ${PROJ_FILES} ${IV_FILES} ${PNG_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/demo/virt_env
)
