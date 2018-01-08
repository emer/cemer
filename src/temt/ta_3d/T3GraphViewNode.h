// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef T3GraphViewNode_h
#define T3GraphViewNode_h 1

// parent includes:
#include <T3NodeParent>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef TA_QT3D

class T3LineBox;

class TA_API T3GraphViewNode: public T3NodeParent {
  Q_OBJECT
  INHERITED(T3NodeParent)
public:
  static float 		frame_margin; // #HIDDEN #NO_SAVE = .05 size of margin around stage for frame
  static float 		frame_width; // #HIDDEN #NO_SAVE = .02 width of frame itself
  static void		initClass();

  float                 width;
  bool	                show_drag;
  T3LineBox*            frame;
  T3Entity*		x_axis;
  T3Entity*		y_axes;
  T3Entity*		z_axis;
  T3Entity*		legend; // can be multiple
  T3Entity*		graphs; // graph boxes
  
  void		        updateNode() override;
  virtual void		setWidth(float wdth);
  void		        setDefaultCaptionTransform() override;

  T3GraphViewNode(Qt3DNode* par = NULL, T3DataView* dataView_ = NULL, float wdth=1.0f,
                  bool show_draggers = true);
  ~T3GraphViewNode();
};

#else // TA_QT3D

class SoScrollBar; // 
class SoFrame; //
class T3TransformBoxDragger; //


taTypeDef_Of(T3GraphViewNode);

class TA_API T3GraphViewNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  TA_SO_NODE_HEADER(T3GraphViewNode);
#endif // def __MAKETA__
public:
  static float 		frame_margin; // #HIDDEN #NO_SAVE = .05 size of margin around stage for frame
  static float 		frame_width; // #HIDDEN #NO_SAVE = .02 width of frame itself
  static void		initClass();

  T3GraphViewNode(T3DataView* dataView_ = NULL, float wdth=1.0f, bool show_draggers = true);
   // dataview is a GraphTableView object

  virtual void		render();
  virtual void		setWidth(float wdth);
  virtual float		getWidth() { return width_; }

  virtual SoSeparator*	x_axis() { return x_axis_sep_; } // #IGNORE 
  virtual SoSeparator*	z_axis() { return z_axis_sep_; } // #IGNORE 
  virtual SoSeparator*	y_axes() { return y_axes_sep_; } // #IGNORE 
  virtual SoSeparator*	legend() { return legend_sep_; } // #IGNORE 
  virtual SoSeparator*	graphs() { return graphs_sep_; } // #IGNORE 

  SoScrollBar*		RowScrollBar() const { return row_scroll_bar_; } // #IGNORE 

protected:
  float			width_;
  SoFrame*		frame_; 

  // in childNodes:
  SoMaterial*		chld_mat_; // first item in childNodes -- default material
  SoSeparator*		x_axis_sep_;
  SoSeparator*		z_axis_sep_;
  SoSeparator*		y_axes_sep_;
  SoSeparator*		legend_sep_; // can be multiple

  SoSeparator*		graphs_sep_; // graph boxes

  bool			show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  SoTransform*		row_sb_tx_; // row scrollbar transform
  SoScrollBar*		row_scroll_bar_;

  ~T3GraphViewNode();
};

#endif // TA_QT3D

#endif // T3GraphViewNode_h
