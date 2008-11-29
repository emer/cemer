########### demo/data_proc install files ###############

FILE(GLOB PROJ_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proj")
FILE(GLOB DAT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.dat")
FILE(GLOB SH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.sh")

install(FILES ${PROJ_FILES} ${DAT_FILES} ${SH_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/demo/data_proc
)
