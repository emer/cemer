# find all of the other packages that Temt/Emergent depend on

find_package(Qt4 REQUIRED QtCore QtGui QtOpenGL QtXml QtNetwork QtWebKit)
find_package(Coin REQUIRED)
find_package(Quarter REQUIRED)
find_package(OpenGL REQUIRED)

if (WIN32)
  # Give FindSubversionLibrary a hint to where the libs are installed on Windows.
  # TODO: verify this just works on its own on Linux/Mac.
  set(SUBVERSION_INSTALL_PATH "${EMER_SVN_LIBS_DIR}")
endif()
find_package(SubversionLibrary REQUIRED)

if (NOT WIN32)
  find_package(Readline REQUIRED)
  if (APPLE)  
    FIND_LIBRARY(CARBON_LIBRARY Carbon)
  else (APPLE)
    find_package(Termcap)
  endif (APPLE)
endif (NOT WIN32)
find_package(ODE)
find_package(GSL)

if (WIN32)
  set(ZLIB_LIBRARIES "")
else (WIN32)
  find_package(ZLIB)
endif (WIN32)

# NOTE: could also do BISON but it is not really required so not worth the hassle
#find_package(BISON)

# setup QT_LIBRARIES, defines, etc through options, and the QT_USE_FILE thing does automagic
set(QT_USE_QT3SUPPORT 0)
set(QT_USE_QTOPENGL 1)
set(QT_USE_QTXML 1)
set(QT_USE_QTNETWORK 1)
set(QT_USE_QTWEBKIT 1)
include(${QT_USE_FILE})

include_directories(${QT_INCLUDES} ${COIN_INCLUDE_DIR} ${QUARTER_INCLUDE_DIR}
  ${ODE_INCLUDE_DIR}
  ${GSL_INCLUDE_DIR}
  ${SUBVERSION_INCLUDE_DIRS}
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
  add_definitions(-DCOIN_DLL -DQUARTER_DLL)
endif (WIN32)

# all dependency libraries to link to -- used automatically in EMERGENT_LINK_LIBRARIES
# specify in executables
set(EMERGENT_DEP_LIBRARIES ${COIN_LIBRARY} ${QUARTER_LIBRARY} ${QT_LIBRARIES}
    ${ODE_LIBRARY} ${GSL_LIBRARIES}
    ${OPENGL_LIBRARIES} ${ZLIB_LIBRARIES}
    ${SUBVERSION_LIBRARIES}
)
if (NOT WIN32)
  set(EMERGENT_DEP_LIBRARIES ${EMERGENT_DEP_LIBRARIES}
    ${READLINE_LIBRARY}
    )

  if (APPLE)  # Termcap on Fedora Core
    set(EMERGENT_DEP_LIBRARIES ${EMERGENT_DEP_LIBRARIES}
      ${CARBON_LIBRARY}
      )
  else (APPLE)
    set(EMERGENT_DEP_LIBRARIES ${EMERGENT_DEP_LIBRARIES}
      ${READLINE_LIBRARY} ${TERMCAP_LIBRARY}
      )
  endif (APPLE)
endif (NOT WIN32)
