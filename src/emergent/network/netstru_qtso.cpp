// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// stuff to implement unit view..

#include "netstru_qtso.h"

#include "ta_geometry.h"
#include "emergent_project.h"
#include "css_qt.h"             // for the cssiSession
#include "ta_qtclipdata.h"
#include "ta_qt.h"
#include "ta_qtgroup.h"
#include "ta_qtviewer.h"

#include "iflowlayout.h"
#include "icolor.h"
#include "ilineedit.h"
#include "iscrollarea.h"

#include "imisc_so.h"
#include "NewNetViewHelper.h"

#include <qapplication.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <QGroupBox>
#include <qlayout.h>
#include <QScrollArea>
#include <qpushbutton.h>
#include <QTreeWidget>
#include <qwidget.h>

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/SoEventManager.h>

#include <limits.h>
#include <float.h>

//////////////////////////
//   ScaleRange         //
//////////////////////////

//////////////////////////
//   nvDataView         //
//////////////////////////

//////////////////////////
//   UnitView           //
//////////////////////////


//////////////////////////
//   UnitGroupView      //
//////////////////////////


////////////////////////////////////////////////////
//   nvhDataView

////////////////////////////////////////////////////
//   LayerView

//////////////////////////
//   PrjnView           //
//////////////////////////

////////////////////////////////////////////////////
//   LayerGroupView


///////////////////////////////////////////////////////////////////////
//      NetViewObjView

//////////////////////////
//   NetView            //
//////////////////////////

// all the slots:


//////////////////////////
//   NetViewPanel       //
//////////////////////////

