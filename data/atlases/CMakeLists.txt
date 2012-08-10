########### proj_templates install files ###############

FILE(GLOB ATLAS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
FILE(GLOB ATLAS_NII_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.nii")

install(FILES ${ATLAS_FILES} ${ATLAS_NII_FILES}
    DESTINATION ${EMERGENT_SHARE_DEST}/data/atlases
)
