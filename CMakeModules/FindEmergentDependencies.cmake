# find all of the other packages that Temt/Emergent depend on

find_package(Qt4 REQUIRED)
find_package(Coin REQUIRED)
find_package(SoQt REQUIRED)
find_package(Readline REQUIRED)
find_package(ODE)
find_package(GSL)

# setup QT_LIBRARIES, defines, etc through options, and the QT_USE_FILE thing does automagic
# TODO: can't figure out why it is still complaining about qt3 support but it is
# need to fix that!
set(QT_USE_QT3SUPPORT 1)
set(QT_USE_QTOPENGL 1)
set(QT_USE_QTXML 1)
include(${QT_USE_FILE})

include_directories(${QT_INCLUDES} ${COIN_INCLUDE_DIR} ${SOQT_INCLUDE_DIR})

# all dependency libraries to link to -- used automatically in EMERGENT_LINK_LIBRARIES
# specify in executables
set(EMERGENT_DEP_LIBRARIES ${COIN_LIBRARY} ${SOQT_LIBRARY} ${QT_LIBRARIES}
    ${READLINE_LIBRARY}
    ${ODE_LIBRARY}
    ${GSL_LIBRARIES}
)
