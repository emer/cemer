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
QT4_WRAP_CPP(emergent_network_SRCS_MOC ${emergent_network_HEADS_MOC})
SET_TA_PROPS(network ${PROJECT_SOURCE_DIR}/src/emergent/network)

# bp
include(${PROJECT_SOURCE_DIR}/src/emergent/bp/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_bp_SRCS ${mypath}/bp "${bp_SRCS}")
ADD_PATH_TO_FILES(emergent_bp_HEADS_MAKETA ${mypath}/bp "${bp_HEADS_MAKETA}")
SET_TA_PROPS(bp ${PROJECT_SOURCE_DIR}/src/emergent/bp)

# leabra
include(${PROJECT_SOURCE_DIR}/src/emergent/leabra/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_leabra_SRCS ${mypath}/leabra "${leabra_SRCS}")
ADD_PATH_TO_FILES(emergent_leabra_HEADS_MAKETA ${mypath}/leabra "${leabra_HEADS_MAKETA}")
SET_TA_PROPS(leabra ${PROJECT_SOURCE_DIR}/src/emergent/leabra)

# cs
include(${PROJECT_SOURCE_DIR}/src/emergent/cs/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_cs_SRCS ${mypath}/cs "${cs_SRCS}")
ADD_PATH_TO_FILES(emergent_cs_HEADS_MAKETA ${mypath}/cs "${cs_HEADS_MAKETA}")
SET_TA_PROPS(cs ${PROJECT_SOURCE_DIR}/src/emergent/cs)

# so
include(${PROJECT_SOURCE_DIR}/src/emergent/so/CMakeFiles.txt)
ADD_PATH_TO_FILES(emergent_so_SRCS ${mypath}/so "${so_SRCS}")
ADD_PATH_TO_FILES(emergent_so_HEADS_MAKETA ${mypath}/so "${so_HEADS_MAKETA}")
SET_TA_PROPS(so ${PROJECT_SOURCE_DIR}/src/emergent/so)

# just list whole set of files here
set(emergentlib_SRCS
	${emergent_network_SRCS} ${emergent_network_SRCS_MOC}
	${emergent_bp_SRCS}
	${emergent_leabra_SRCS}
	${emergent_cs_SRCS}
	${emergent_so_SRCS}
)

EMERGENT_ADD_LIBRARY(emergentlib SHARED ${emergentlib_SRCS})

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

