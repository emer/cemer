########### proj_templates install files ###############

FILE(GLOB ATLAS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.nii")

install(FILES ${ATLAS_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/data/atlases
)
