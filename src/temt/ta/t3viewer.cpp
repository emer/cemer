// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.



// t3viewer.cc

#include "t3viewer.h"

#include "ta_qt.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_qtclipdata.h"
#include "ta_qtdialog.h"

#include "css_machine.h" // for trace flag

#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QLayout>
#include <QMenu>
#include <QScrollBar>
#include <QTabWidget>
#include <QTimer>
#include <QPushButton>
#include <QGLWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QKeyEvent>

#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/annex/HardCopy/SoVectorizePSAction.h>
#include <Inventor/events/SoButtonEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/misc/SoBase.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/SoEventManager.h>
#include <Inventor/SoRenderManager.h>
# include <QGLPixelBuffer>

#include "qtthumbwheel.h"



using namespace Qt;

// from: http://doc.trolltech.com/4.3/opengl-samplebuffers-glwidget-cpp.html
#ifndef GL_MULTISAMPLE
# define GL_MULTISAMPLE  0x809D
#endif

//////////////////////////////

#include "pick.xpm"
#include "view.xpm"
// #include "home.xpm"
// #include "set_home.xpm"
#include "seek.xpm"
#include "view_all.xpm"
#include "print.xpm"
#include "snapshot.xpm"

#define WHEEL_LENGTH 60         // long axis
#define WHEEL_WIDTH 20          // short axis
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 20

// for thumb wheel -- max value and wraparound delta threshold detection
#define THUMB_MAX_VAL 1000
#define THUMB_INIT_VAL 500
#define THUMB_PAGE_STEP 10
#define THUMB_WRAP_THR 800

/////////////////////////////////////////////////////////////
//              Saved Views



/////////////////////////////////////////////////////////////
//              Quarter Widget

/////////////////////////////////////////////////////////////
//              Examiner Viewer



//////////////////////////
//   iSoSelectionEvent  //
//////////////////////////

//////////////////////////////////
//    iT3ViewspaceWidget        //
//////////////////////////////////


//////////////////////////
//   iT3DataViewFrame   //
//////////////////////////


//////////////////////////
//      T3DataViewFrame //
//////////////////////////


//////////////////////////
//   iTabBarEx          //
//////////////////////////


//////////////////////////
//   iT3DataViewer      //
//////////////////////////

