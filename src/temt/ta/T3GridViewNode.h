// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef T3GridViewNode_h
#define T3GridViewNode_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:

// declare all other types mentioned but not required to include:
class SoFrame; // 
class SoGroup; // 
class SoMaterial; // 
class SoScrollBar; // 
class T3TransformBoxDragger; //


class TA_API T3GridViewNode: public T3NodeLeaf {
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3GridViewNode);
#endif // def __MAKETA__
public:
  static float 		frame_margin; // = .05 size of margin around stage for frame
  static float 		frame_width; // = .02 width of frame itself
  static void		initClass();

  SoFrame*		frame() const {return frame_;} 
  SoGroup*		grid() const {return grid_;}
  SoMaterial*		matStage() const {return mat_stage_;}
  SoSeparator*		header() const {return header_;}
  SoSeparator*		body() const {return body_;}

  SoScrollBar*		ColScrollBar() const { return col_scroll_bar_; }
  SoScrollBar*		RowScrollBar() const { return row_scroll_bar_; }

  virtual void		render();
  virtual void		setWidth(float wdth);
  virtual float		getWidth() { return width_; }

  override void		addRemoveChildNode(SoNode* node, bool adding) {} // ignore 
  
  T3GridViewNode(T3DataView* dataView_ = NULL, float wdth=1.0f, bool show_draggers = true);
  // dataview is a GridTableView object

protected:
  float			width_;
  SoSeparator*		stage_;
  SoMaterial*		mat_stage_;
  SoTranslation*	  txlt_stage_;
  SoSeparator*		  header_;
  SoSeparator*		  body_;//
  SoSeparator*		  grid_;
  SoFrame*		frame_; 

  bool			show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  SoScrollBar*		col_scroll_bar_;
  SoTransform*		col_sb_tx_; // col scrollbar transform
  SoScrollBar*		row_scroll_bar_;

  ~T3GridViewNode();
};

#endif // T3GridViewNode_h
