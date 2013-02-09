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

#ifndef T3LayerGroupNode_h
#define T3LayerGroupNode_h 1

// parent includes:
#include "network_def.h"
#include <T3NodeParent>

// member includes:
#include <iVec3i>
#include <iVec3f>

// declare all other types mentioned but not required to include:
class SoLineBox3d;
class SoDrawStyle;
class SoTranslate1Dragger;
class SoTranslate2Dragger;


class E_API T3LayerGroupNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3LayerGroupNode);
#endif
public:
  static void		initClass();

  void 			setGeom(int px, int py, int pz, 
				float lg_max_x, float lg_max_y, float lg_max_z,
				float max_x, float max_y, float max_z);

  bool			hideLines() { return hide_lines_; }
  SoDrawStyle* 		drawStyle() { return drw_styl_; } // #IGNORE

  T3LayerGroupNode(T3DataView* dataView_ = NULL, bool show_draggers = true,
		   bool hide_lines = false, bool mode_2d = false);

protected:
  
  iVec3i		pos; 	// starting position
  iVec3i		lgp_max_size; 	// layer group max_size
  iVec3f		max_size; // maximum size of network x,y,z

  void			render(); // called after pos/geom changes
  ~T3LayerGroupNode();

private:
  SoLineBox3d*		shape_;
  SoDrawStyle*		drw_styl_;

  bool			show_drag_;
  bool			hide_lines_;
  bool			mode_2d_;

  SoSeparator*		xy_drag_sep_;
  SoTransform*		xy_drag_xf_;
  SoTranslate2Dragger*	xy_dragger_;
  SoCalculator*		xy_drag_calc_;

  SoSeparator*		z_drag_sep_;
  SoTransform*		z_drag_xf_;
  SoTranslate1Dragger*	z_dragger_;
  SoCalculator*		z_drag_calc_;
};

#endif // T3LayerGroupNode_h
