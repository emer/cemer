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



// netstru_so.h -- inventor controls/objects for network structures

// NOTE: functions inside "ifdef GUI" blocks are implemented in netstru_qtso.cc

#ifndef NETSTRU_SO_H
#define NETSTRU_SO_H

// #include "network_def.h"
// #include "t3node_so.h"

// // externals
// class SbVec3f; // #IGNORE
// class SoAction; // #IGNORE
// class SoBaseColor; // #IGNORE
// class SoCone; // #IGNORE
// class SoCube; // #IGNORE
// class SoCylinder; // #IGNORE
// class SoFont; // #IGNORE
// class SoIndexedTriangleStripSet; // #IGNORE
// class SoVertexProperty;		 // #IGNORE
// class SoTranslate2Dragger; // #IGNORE
// class SoTransformBoxDragger; // #IGNORE
// class SoCalculator; // #IGNORE
// class SoComplexity; // #IGNORE
// class SoIndexedLineSet; // #IGNORE
// class SoDrawStyle; // #IGNORE

// // forwards
// class T3UnitNode;
// class T3UnitGroupNode;
// class T3LayerNode;
// class T3PrjnNode;
// class T3LayerGroupNode;
// class T3NetNode;


////////////////////////////////////////////////////
//   T3UnitNode	

/*
    this: SoSeparator - the object itself
      transform: SoTransform
      material: SoMaterial
      [drawStyle: SoDrawStyle] (inserted when picked, to make wireframe instead of solid)
      shape: SoCylinder (could be changed to something else)
*/

////////////////////////////////////////////////////
//   T3UnitGroupNode	


////////////////////////////////////////////////////
//   T3LayerNode


////////////////////////////////////////////////////
//   T3PrjnNode	


////////////////////////////////////////////////////
//   T3LayerGroupNode

////////////////////////////////////////////////////
//   T3NetViewObjNode


////////////////////////////////////////////////////
//   T3NetNode

#endif // NETSTRU_SO_H

