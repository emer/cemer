// Carnegie Mellon University, Princeton University.
// Copyright, 1995-2007, Regents of the University of Colorado,
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



#include "ta_datatable_qtso.h"

// stuff to implement graphical view of datatable
// #include "igeometry.h"

// #include "ta_qtgroup.h"
// #include "ta_qtclipdata.h"

// #include "ta_datatable_so.h"
// #include "ta_math.h"
// #include "ta_project.h"

// #include "ilineedit.h"
// #include "ispinbox.h"
// #include "iscrollarea.h"
// #include "iflowlayout.h"
// #include "NewNetViewHelper.h"

// #include <QApplication>
// #include <QButtonGroup>
// #include <QCheckBox>
// #include <QClipboard>
// #include <qimage.h>
// #include <qlabel.h>
// #include <qlayout.h>
// #include <qpalette.h>
// #include <qpixmap.h>
// #include <QPushButton>
// #include <QTableView>
// #include <QTextStream>
// #include <QHeaderView>

// #include <Inventor/SbLinear.h>
// #include <Inventor/fields/SoMFString.h>
// #include <Inventor/nodes/SoAsciiText.h>
// #include <Inventor/nodes/SoBaseColor.h>
// #include <Inventor/nodes/SoCube.h>
// #include <Inventor/nodes/SoDirectionalLight.h>
// #include <Inventor/nodes/SoFont.h>
// #include <Inventor/nodes/SoLightModel.h>
// #include <Inventor/nodes/SoMaterial.h>
// #include <Inventor/nodes/SoPerspectiveCamera.h>
// #include <Inventor/nodes/SoSelection.h>
// #include <Inventor/nodes/SoSeparator.h>
// #include <Inventor/nodes/SoTransform.h>
// #include <Inventor/nodes/SoTranslation.h>
// #include <Inventor/nodes/SoComplexity.h>
// #include <Inventor/nodes/SoText2.h>
// #include <Inventor/draggers/SoTransformBoxDragger.h>
// #include <Inventor/nodes/SoEventCallback.h>
// #include <Inventor/events/SoMouseButtonEvent.h>
// #include <Inventor/actions/SoRayPickAction.h>
// #include <Inventor/SoPickedPoint.h>
// #include <Inventor/SoEventManager.h>

// #include <limits.h>
// #include <float.h>

// #define DIST(x,y) sqrt((double) ((x * x) + (y*y)))

//////////////////////////////////
//   DataTableModel             //
//////////////////////////////////


//////////////////////////
//   DataTable (gui)    //
//////////////////////////


//////////////////////////
//   DataColView        //
//////////////////////////



//////////////////////////
// DataTableView        //
//////////////////////////


///////////////////////////////////////////////////////////////////////////////
//      Grid View

//////////////////////////////////
//   GridColView                //
//////////////////////////////////


//////////////////////////////////
//  GridTableView               //
//////////////////////////////////

// Add a new GridTableView object to the frame for the given DataTable.

///////////////////////////////////////////////////////////////
//      Actual Rendering of grid display
//
// margins are applied *within* cells -- cells are a specific height and width
// and then contents are indented by margin size
// grids can be drawn exactly on width/height boundaries


////////////////////////////////////////////////////////////////////////
// note on following: basically callbacks from view..


//////////////////////////
//    iTableView_Panel //
//////////////////////////


//////////////////////////
// iGridTableView_Panel //
//////////////////////////


///////////////////////////////////////////////////////////////////////////////
//      Graph View


//////////////////////////////////
//   GraphColView               //
//////////////////////////////////


//////////////////////////
//      GraphAxisBase   //
//////////////////////////


///////////////////////////////////////////////////
//      Range Management


/////////////////////////////////////////////////////////
//      rendering


//////////////////////////////////
//  GraphPlotView               //
//////////////////////////////////


//////////////////////////////////
//  GraphAxisView               //
//////////////////////////////////


//////////////////////////////////
//  GraphTableView              //
//////////////////////////////////

//////////////////////////
// iGraphTableView_Panel //
//////////////////////////


/////////////////////////////////////////////////////////////////////////////////
//              Other GUI stuff

//////////////////////////
// tabDataTableViewType //
//////////////////////////


//////////////////////////
//   iDataTableView     //
//////////////////////////


//////////////////////////
//    DataTableDelegate //
//////////////////////////

DataTableDelegate::DataTableDelegate(DataTable* dt_)
:inherited(NULL)
{
  dt = dt_;
}

DataTableDelegate::~DataTableDelegate() {
}


//////////////////////////
//    iDataTableEditor  //
//////////////////////////


//////////////////////////
//    iDataTablePanel   //
//////////////////////////


//////////////////////////////////
//  taiTabularDataMimeFactory   //
//////////////////////////////////


//////////////////////////////////
//  taiTabularDataMimeItem      //
//////////////////////////////////


//////////////////////////////////
//  taiMatrixDataMimeItem       //
//////////////////////////////////


//////////////////////////////////
//  taiTsvMimeItem              //
//////////////////////////////////


//////////////////////////////////
//  taiTableDataMimeItem                //
//////////////////////////////////

