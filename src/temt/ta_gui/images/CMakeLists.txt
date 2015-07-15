########### temt images install files ###############

FILE(GLOB PNG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.png")

install(FILES ${PNG_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/images
)
