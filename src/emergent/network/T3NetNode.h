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

#ifndef T3NetNode_h
#define T3NetNode_h 1

// parent includes:
#include "network_def.h"
#include <T3NodeParent>

// member includes:

// declare all other types mentioned but not required to include:
class SoLineBox3d;
class T3TransformBoxDragger;

class EMERGENT_API T3NetNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3NetNode);
#endif
public:
  static void		initClass();

  SoDrawStyle*		shapeDraw() {return shape_draw_;}
  SoSeparator*		netText() {return net_text_;} // extra text of network vars etc
  SoTransform* 		netTextXform() {return net_text_xform_;}
  SoTransform* 		netTextDragXform() {return net_text_drag_xform_;}
  bool			netTextDrag() {return show_net_text_drag_;}
  SoSeparator*		wtLinesSep() {return wt_lines_;}
  SoDrawStyle*		wtLinesDraw() {return wt_lines_draw_;}
  SoIndexedLineSet*	wtLinesSet() {return wt_lines_set_;}
  SoVertexProperty* 	wtLinesVtxProp() {return wt_lines_vtx_prop_;}

  T3NetNode(T3DataView* dataView_ = NULL, bool show_draggers = true,
	    bool show_net_text = true, bool show_nt_drag = true,
	    bool mode_2d = false);

protected:
  void		setDefaultCaptionTransform(); // override
  ~T3NetNode();

protected:
  SoDrawStyle*		shape_draw_;
  SoLineBox3d*		shape_; //#IGNORE
  SoSeparator* 		net_text_; // network text variables
  SoSeparator* 		wt_lines_; // weight lines
  SoDrawStyle*		wt_lines_draw_;
  SoIndexedLineSet*	wt_lines_set_;
  SoVertexProperty*	wt_lines_vtx_prop_;

  bool			show_drag_;
  bool			show_net_text_drag_;
  bool			mode_2d_;
  T3TransformBoxDragger* drag_;	// my position dragger
  T3TransformBoxDragger* net_text_drag_;	// my net text dragger
  SoTransform*		net_text_xform_;
  SoTransform*		net_text_drag_xform_;
};

#endif // T3NetNode_h
