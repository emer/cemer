########### proj_templates install files ###############

FILE(GLOB STD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.nii")

install(FILES ${STD_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/data/standard
)
