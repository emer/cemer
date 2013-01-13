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



// t3viewer.h: basic types for objects that can be independently viewed in space

#ifndef T3VIEWER_H
#define T3VIEWER_H

// #include "ta_group.h"
// #include "ta_qt.h"
// #include "ta_qtviewer.h"
// #include "t3node_so.h"
// #include "ta_def.h"

// #include "ta_TA_type_WRAPPER.h"

// //#include "igeometry.h"
// #include "ta_geometry.h"
// #include "safeptr_so.h"

// #ifndef __MAKETA__
// # include <qevent.h>
// # include <qwidget.h>
// # include <Inventor/nodes/SoSeparator.h>
// # include <Inventor/SbLinear.h>
// # include <Quarter/Quarter.h>
// # include <Quarter/QuarterWidget.h>

// using SIM::Coin3D::Quarter::QuarterWidget;
// #else
// class QuarterWidget;    // #IGNORE
// class QGLWidget;    // #IGNORE
// class QGLFormat;    // #IGNORE
// class QGLContext;    // #IGNORE
// #endif

// // externals
// class taiClipData;
// class taiMimeItem;
// class taiMimeSource;
// class taVector3i;
// class taTransform;
// class SoPath; // #IGNORE
// class SoCamera; // #IGNORE
// class SbViewportRegion; // #IGNORE
// class T3Node;
// class QGLPixelBuffer; // #IGNORE
// class QGLFormat; // #IGNORE
// class SoRenderManager; // #IGNORE

// // forwards
// class T3DataView;
// class T3DataViewPar;
// class T3DataViewRoot;
// class T3DataViewMain;

// class T3DataView_List;
// class iT3ViewspaceWidget;
// class iSoSelectionEvent; // #IGNORE
// class iT3DataViewFrame; // #IGNORE
// class iT3DataViewer; // #IGNORE
// class T3DataViewFrame;
// class T3DataViewer;
// class T3ExaminerViewer;

// class QtThumbWheel;             // #IGNORE

// SoPtr_Of(T3Node);


//////////////////////////////////////////////////////////////////////////////
//   T3ExaminerViewer -- customized

// note: this now requires Quarter instead of SoQt


// #include <T3DataView>
// #include <T3DataViewPar>
// #include <T3DataViewRoot>
// #include <T3DataViewMain>

//////////////////////////
//   iSoSelectionEvent  //
//////////////////////////


//////////////////////////
//   iT3ViewspaceWidget //
//////////////////////////

/*
  Scene graph:
  SoSeparator* root_so
    SoGroup*   callbacks -- we put any callback nodes in here
    SoSeparator* items -- the actual items get rendered

*/

//////////////////////////
//   iT3DataViewFrame   //
//////////////////////////


//////////////////////////
//   iT3DataViewer      //
//////////////////////////

// #include <T3DataViewer>

#endif


