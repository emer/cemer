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

#include "netstru_so.h"

//incl the coin header to get gl.h #include <GL/gl.h>
#include <Inventor/system/gl.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodes/SoIndexedLineSet.h>

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////////////
//	  T3UnitNode		//
//////////////////////////////////

//////////////////////////
//   T3UnitNode_Cylinder//
//////////////////////////


//////////////////////////
//   T3UnitNode_Block//
//////////////////////////

// note: Block is never called now because it is done in optimized form by the unit group

//////////////////////////
//   T3UnitNode_Rect//
//////////////////////////


//////////////////////////
//   T3UnitGroupNode	//
//////////////////////////


////////////////////////////////////////////////////
//   T3LayerNode


//////////////////////////////////
//	  T3PrjnNode		//
//////////////////////////////////


////////////////////////////////////////////////////
//   T3LayerGroupNode


/////////////////////////////////////////////
//	NetViewObj

//////////////////////////
//   T3NetNode		//
//////////////////////////

