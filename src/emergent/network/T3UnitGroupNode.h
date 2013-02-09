// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef T3UnitGroupNode_h
#define T3UnitGroupNode_h 1

// parent includes:
#include "network_def.h"
#include <T3NodeParent>
#include <iVec2i>
#include <iVec3f>

// member includes:

// declare all other types mentioned but not required to include:

class E_API T3UnitGroupNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3UnitGroupNode);
#endif // def __MAKETA__
public:
  static float		height; // nominal amount of height, so doesn't vanish
  static float		inset; // amount inset from the layer

  static void		initClass();
#ifndef __MAKETA__
  static void		shapeCallback(void* data, SoAction* act);
#endif
  static void		drawGrid(T3UnitGroupNode* node);

//  void			clearUnits(); // remove all units
//  void			addUnit(int x, int y); // remove all units
  SoFont*		unitCaptionFont(bool auto_create = false); // #IGNORE

  void 			setGeom(int px, int py, float max_x, float max_y, float max_z,
				float disp_scale);

  // sets (actual) geom of group; creates/positions units; setes max_size
  SoIndexedTriangleStripSet* shape() {return shape_;} // #IGNORE
  SoVertexProperty* 	vtxProp() {return vtx_prop_;} // #IGNORE
  SoSeparator*		unitText() {return unit_text_;} // #IGNORE extra text of unit vars
  SoSeparator*		getUnitText();		      // #IGNORE get a new unit text separator
  void			removeUnitText();	      // remove unit text separator

  SoSeparator*		snapBordSep() {return snap_bord_;} // #IGNORE
  SoDrawStyle*		snapBordDraw() {return snap_bord_draw_;} // #IGNORE
  SoIndexedLineSet*	snapBordSet() {return snap_bord_set_;} // #IGNORE
  SoVertexProperty* 	snapBordVtxProp() {return snap_bord_vtx_prop_;} // #IGNORE

  T3UnitGroupNode(T3DataView* dataView_ = NULL, bool no_unts = false);

protected:
  iVec2i		geom; //note, not a field
  float			disp_scale;
  iVec2i		scaled_geom; // layer disp_scale * geom
  iVec3f		max_size; // maximum size of network x,y,z
  SoFont*		unitCaptionFont_;
  bool			no_units; // summary mode: no unit objects exist

  SoSeparator* 		snap_bord_; // weight lines
  SoDrawStyle*		snap_bord_draw_;
  SoIndexedLineSet*	snap_bord_set_;
  SoVertexProperty*	snap_bord_vtx_prop_;

  ~T3UnitGroupNode();

protected:
  SoIndexedTriangleStripSet*	shape_;
  SoVertexProperty*	vtx_prop_;
  SoSeparator* 		unit_text_; // unit text variables
};

#endif // T3UnitGroupNode_h
