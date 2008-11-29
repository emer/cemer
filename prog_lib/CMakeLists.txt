########### prog_lib install files ###############

FILE(GLOB PROG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.prog")
FILE(GLOB PROG_GP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.progp")

install(FILES ${PROG_FILES} ${PROG_GP_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/prog_lib
)
