# find all of the other packages that Temt/Emergent depend on

find_package(Qt4 REQUIRED)
find_package(Coin REQUIRED)
find_package(SoQt REQUIRED)
if (NOT WIN32)
  find_package(Readline REQUIRED)
  if (NOT APPLE)  
    find_package(Termcap)
  endif (NOT APPLE)
endif (NOT WIN32)
find_package(ODE)
find_package(GSL)

# NOTE: could also do BISON but it is not really required so not worth the hassle
#find_package(BISON)

# setup QT_LIBRARIES, defines, etc through options, and the QT_USE_FILE thing does automagic
# TODO: can't figure out why it is still complaining about qt3 support but it is
# need to fix that!
set(QT_USE_QT3SUPPORT 1)
set(QT_USE_QTOPENGL 1)
set(QT_USE_QTXML 1)
set(QT_USE_QTNETWORK 1)
set(QT_USE_QTWEBKIT 1)
include(${QT_USE_FILE})

include_directories(${QT_INCLUDES} ${COIN_INCLUDE_DIR} ${SOQT_INCLUDE_DIR}
  ${ODE_INCLUDE_DIR}
  ${GSL_INCLUDE_DIR}
)
if (WIN32)
  #note: valid in main app and plugin contexts:
  include_directories(${EMERGENT_SHARE_DIR}/3rdparty/include
    $ENV{COINDIR}/include
  )
else (WIN32)
  include_directories(${READLINE_INCLUDE_DIR} )
  # Termcap on Fedora Core
  if (NOT APPLE)  
    include_directories(${TERMCAP_INCLUDE_DIR} )
  endif (NOT APPLE)
endif (WIN32)

# Windows dll macros
if (WIN32)
  add_definitions(-DCOIN_DLL -DSOQT_DLL)
endif (WIN32)

# all dependency libraries to link to -- used automatically in EMERGENT_LINK_LIBRARIES
# specify in executables
set(EMERGENT_DEP_LIBRARIES ${COIN_LIBRARY} ${SOQT_LIBRARY} ${QT_LIBRARIES}
    ${ODE_LIBRARY}
    ${GSL_LIBRARIES}
)
if (NOT WIN32)
  set(EMERGENT_DEP_LIBRARIES ${EMERGENT_DEP_LIBRARIES}
    ${READLINE_LIBRARY}
    )

  if (NOT APPLE)  # Termcap on Fedora Core
    set(EMERGENT_DEP_LIBRARIES ${EMERGENT_DEP_LIBRARIES}
      ${READLINE_LIBRARY} ${TERMCAP_LIBRARY}
      )
  endif (NOT APPLE)
endif (NOT WIN32)
