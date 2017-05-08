########### #patch_lib install files ###############

#  comment to update

FILE(GLOB PATCH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.patch")

install(FILES ${PATCH_FILES} 
    DESTINATION ${EMERGENT_SHARE_DEST}/patch_lib
)
