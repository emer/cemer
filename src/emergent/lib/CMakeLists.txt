########### emergent lib target ###############

# this is where all the emergent code gets built into a single library
# we read in the list of files from the relevant directories and then 
# add relative path for here, then just list them all as compile targets
# also, all moc generation happens here, but all maket generation must
# happen in the original source directory!!

# cmake 2.4 needs absolute pathing to find _TA.cpp file
#set(mypath "..")
set(mypath ${PROJECT_SOURCE_DIR}/src/emergent)

# network
include(${PROJECT_SOURCE_DIR}/src/emergent/network/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_network_SRCS ${mypath}/network "${network_SRCS}")
ADD_PATH_TO_FILES(emergent_network_HEADS_MOC ${mypath}/network "${network_HEADS_MOC}")
ADD_PATH_TO_FILES(emergent_network_HEADS_MAKETA ${mypath}/network "${network_HEADS_MAKETA}")
ADD_PATH_TO_FILES(emergent_network_HEADS ${mypath}/network "${network_HEADS}")
SET_TA_PROPS(network ${PROJECT_SOURCE_DIR}/src/emergent/network)

MAKETA_WRAP_H(emergent_network_SRCS_MAKETA ${emergent_network_HEADS_MAKETA} OPTIONS network)

ADD_PATH_TO_FILES(emergent_network_HEADS_MAKETA_MI ${mypath}/network "${network_HEADS_MAKETA_MI}")
ADD_PATH_TO_FILES(emergent_network_HEADS_MAKETA_MI_DEPS ${mypath}/network "${network_HEADS_MAKETA_MI_DEPS}")

MAKETA_WRAP_H_MI(emergent_network_SRCS_MAKETA_MI "${emergent_network_HEADS_MAKETA_MI}"
  "${emergent_network_HEADS_MAKETA_MI_DEPS}")

# bp
include(${PROJECT_SOURCE_DIR}/src/emergent/bp/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_bp_SRCS ${mypath}/bp "${bp_SRCS}")
ADD_PATH_TO_FILES(emergent_bp_HEADS_MAKETA ${mypath}/bp "${bp_HEADS_MAKETA}")
ADD_PATH_TO_FILES(emergent_bp_HEADS ${mypath}/bp "${bp_HEADS}")
SET_TA_PROPS(bp ${PROJECT_SOURCE_DIR}/src/emergent/bp)

MAKETA_WRAP_H(emergent_bp_SRCS_MAKETA ${emergent_bp_HEADS_MAKETA} OPTIONS bp)

# leabra
include(${PROJECT_SOURCE_DIR}/src/emergent/leabra/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_leabra_SRCS ${mypath}/leabra "${leabra_SRCS}")
ADD_PATH_TO_FILES(emergent_leabra_HEADS_MAKETA ${mypath}/leabra "${leabra_HEADS_MAKETA}")
ADD_PATH_TO_FILES(emergent_leabra_HEADS ${mypath}/leabra "${leabra_HEADS}")
ADD_PATH_TO_FILES(emergent_leabra_CUDA ${mypath}/leabra "${leabra_CUDA}")
SET_TA_PROPS(leabra ${PROJECT_SOURCE_DIR}/src/emergent/leabra)

MAKETA_WRAP_H(emergent_leabra_SRCS_MAKETA ${emergent_leabra_HEADS_MAKETA} OPTIONS leabra)

ADD_PATH_TO_FILES(emergent_leabra_HEADS_MAKETA_MI ${mypath}/leabra "${leabra_HEADS_MAKETA_MI}")
ADD_PATH_TO_FILES(emergent_leabra_HEADS_MAKETA_MI_DEPS ${mypath}/leabra "${leabra_HEADS_MAKETA_MI_DEPS}")

IF(CUDA_BUILD)
CUDA_WRAP_SRCS(emergentlib OBJ emergent_leabra_CUDA_OBJS "${emergent_leabra_CUDA}")
ENDIF(CUDA_BUILD)

MAKETA_WRAP_H_MI(emergent_leabra_SRCS_MAKETA_MI "${emergent_leabra_HEADS_MAKETA_MI}"
  "${emergent_leabra_HEADS_MAKETA_MI_DEPS}")

# cs
include(${PROJECT_SOURCE_DIR}/src/emergent/cs/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_cs_SRCS ${mypath}/cs "${cs_SRCS}")
ADD_PATH_TO_FILES(emergent_cs_HEADS_MAKETA ${mypath}/cs "${cs_HEADS_MAKETA}")
ADD_PATH_TO_FILES(emergent_cs_HEADS ${mypath}/cs "${cs_HEADS}")
SET_TA_PROPS(cs ${PROJECT_SOURCE_DIR}/src/emergent/cs)

MAKETA_WRAP_H(emergent_cs_SRCS_MAKETA ${emergent_cs_HEADS_MAKETA} OPTIONS cs)

# so
include(${PROJECT_SOURCE_DIR}/src/emergent/so/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_so_SRCS ${mypath}/so "${so_SRCS}")
ADD_PATH_TO_FILES(emergent_so_HEADS_MAKETA ${mypath}/so "${so_HEADS_MAKETA}")
ADD_PATH_TO_FILES(emergent_so_HEADS ${mypath}/so "${so_HEADS}")
SET_TA_PROPS(so ${PROJECT_SOURCE_DIR}/src/emergent/so)

MAKETA_WRAP_H(emergent_so_SRCS_MAKETA ${emergent_so_HEADS_MAKETA} OPTIONS so)

# actr
include(${PROJECT_SOURCE_DIR}/src/emergent/actr/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_actr_SRCS ${mypath}/actr "${actr_SRCS}")
ADD_PATH_TO_FILES(emergent_actr_HEADS_MAKETA ${mypath}/actr "${actr_HEADS_MAKETA}")
ADD_PATH_TO_FILES(emergent_actr_HEADS ${mypath}/actr "${actr_HEADS}")
SET_TA_PROPS(actr ${PROJECT_SOURCE_DIR}/src/emergent/actr)

MAKETA_WRAP_H(emergent_actr_SRCS_MAKETA ${emergent_actr_HEADS_MAKETA} OPTIONS actr)

# virt_env
include(${PROJECT_SOURCE_DIR}/src/emergent/virt_env/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_virt_env_SRCS ${mypath}/virt_env "${virt_env_SRCS}")
ADD_PATH_TO_FILES(emergent_virt_env_HEADS_MOC ${mypath}/virt_env "${virt_env_HEADS_MOC}")
ADD_PATH_TO_FILES(emergent_virt_env_HEADS_MAKETA ${mypath}/virt_env "${virt_env_HEADS_MAKETA}")
ADD_PATH_TO_FILES(emergent_virt_env_HEADS ${mypath}/virt_env "${virt_env_HEADS}")
SET_TA_PROPS(virt_env ${PROJECT_SOURCE_DIR}/src/emergent/virt_env)

MAKETA_WRAP_H(emergent_virt_env_SRCS_MAKETA ${emergent_virt_env_HEADS_MAKETA} OPTIONS virt_env)

ADD_PATH_TO_FILES(emergent_virt_env_QRC ${mypath}/virt_env "${virt_env_QRC}")

if (QT_USE_5)
  qt5_wrap_cpp(emergent_network_SRCS_MOC ${emergent_network_HEADS_MOC})
  qt5_wrap_cpp(emergent_virt_env_SRCS_MOC ${emergent_virt_env_HEADS_MOC})

  qt5_add_resources(emergent_virt_env_SRCS_QRC ${emergent_virt_env_QRC})
  
else (QT_USE_5)
  QT4_WRAP_CPP(emergent_network_SRCS_MOC ${emergent_network_HEADS_MOC})
  QT4_WRAP_CPP(emergent_virt_env_SRCS_MOC ${emergent_virt_env_HEADS_MOC})
endif (QT_USE_5)

# just list whole set of files here -- including headers for IDE's
set(emergentlib_SRCS
  ${emergent_network_SRCS} ${emergent_network_SRCS_MOC}
  ${emergent_network_SRCS_MAKETA} ${emergent_network_SRCS_MAKETA_MI}
  ${emergent_bp_SRCS} ${emergent_bp_SRCS_MAKETA}
  ${emergent_leabra_SRCS} ${emergent_leabra_SRCS_MAKETA} ${emergent_leabra_SRCS_MAKETA_MI}
  ${emergent_cs_SRCS} ${emergent_cs_SRCS_MAKETA}
  ${emergent_so_SRCS} ${emergent_so_SRCS_MAKETA}
  ${emergent_actr_SRCS} ${emergent_actr_SRCS_MAKETA}
  ${emergent_virt_env_SRCS} ${emergent_virt_env_SRCS_MOC}
  ${emergent_virt_env_SRCS_MAKETA}

  ${emergent_network_HEADS}
  ${emergent_bp_HEADS}
  ${emergent_leabra_HEADS}
  ${emergent_cs_HEADS}
  ${emergent_so_HEADS}
  ${emergent_actr_HEADS}
  ${emergent_leabra_CUDA_OBJS}
  ${emergent_virt_env_HEADS}

  ${emergent_virt_env_SRCS_QRC}
)

EMERGENT_ADD_LIBRARY(emergentlib SHARED ${emergentlib_SRCS})
if (WIN32)
  set_target_properties(emergentlib PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif (WIN32)

# experiment for vectormath.h intel shared lib -- avoid at all costs and use a 
# lookup table instead
# add_library( libsvml SHARED IMPORTED )
# set_target_properties( libsvml PROPERTIES IMPORTED_LOCATION /usr/local/lib/libsvml.dylib )
# TARGET_LINK_LIBRARIES(emergentlib libsvml)

if (WIN32)
  # Don't warn so much on 3rd party C files.
  set_source_files_properties(
    ${PROJECT_SOURCE_DIR}/src/emergent/network/nifti1_io.c
    ${PROJECT_SOURCE_DIR}/src/emergent/network/znzlib.c
    PROPERTIES
    COMPILE_FLAGS "/D_CRT_SECURE_NO_WARNINGS /wd4101 /wd4244 /wd4267"
  )
endif (WIN32)

# 2nd arg is extra libs to link to
EMERGENT_LINK_LIBRARIES(emergentlib temt)

# Windows import/export control
if (WIN32)
  add_definitions(-DTA_DLL -DEMERGENT_DLL -DEMERGENT_EXPORTS)
endif (WIN32)

########### install files ###############

if (WIN32)
  install(TARGETS emergentlib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
  )
else (WIN32)
  install(TARGETS emergentlib
    LIBRARY DESTINATION lib
  )
endif (WIN32)

