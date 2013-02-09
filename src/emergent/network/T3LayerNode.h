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

#ifndef T3LayerNode_h
#define T3LayerNode_h 1

// parent includes:
#include "network_def.h"
#include <T3NodeParent>

// member includes:
#include <iVec2i>
#include <iVec3f>

// declare all other types mentioned but not required to include:
class SoTranslate2Dragger; // #IGNORE
class SoTranslate1Dragger; // #IGNORE
class SoTransform; // #IGNORE
class SoCalculator; // #IGNORE
class SoSeparator; // #IGNORE
class SoFrame; // #IGNORE


class E_API T3LayerNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3LayerNode);
#endif
public:
  static const float 	height; // = 0.05f height of layer frame shape itself (in fractions of a unit)
  static const float 	width; // = 0.5f width of layer frame shape (in frac of unit)
  static const float 	max_width; // = 0.05f maximum absolute width value (prevent mondo frames for very small nets)

  static void		initClass();

  void 			setGeom(int px, int py, float max_x, float max_y, float max_z, float disp_scale);

  T3LayerNode(T3DataView* dataView_ = NULL, bool show_draggers = true,
	      bool mode_2d = false);

protected:
  
  iVec2i		geom; //note, not a field
  float			disp_scale;
  iVec2i		scaled_geom; // disp_scale * geom
  iVec3f		max_size; // maximum size of network x,y,z
  void			render(); // called after pos/geom changes
  ~T3LayerNode();

private:
  SoFrame*		shape_;

  bool			show_drag_;
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

#endif // T3LayerNode_h
