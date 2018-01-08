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

#ifndef T3GridViewNode_h
#define T3GridViewNode_h 1

// parent includes:
#include <T3NodeParent>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef TA_QT3D

class T3LineStrip;
class T3LineBox;

class TA_API T3GridViewNode: public T3NodeParent {
  Q_OBJECT
  INHERITED(T3NodeParent)
public:
  static float 		frame_margin; // #HIDDEN #NO_SAVE = .05 size of margin around stage for frame
  static float 		frame_width; // #HIDDEN #NO_SAVE = .02 width of frame itself

  float                 width;
  bool                  show_drag;
  T3LineBox*            frame;
  T3LineStrip*          grid;
  T3Entity*             header;
  T3Entity*             body;

  void		        updateNode() override;
  virtual void		setWidth(float wdth);
  void		        setDefaultCaptionTransform() override;

  T3GridViewNode(Qt3DNode* par = NULL, T3DataView* dataView_ = NULL,
                 float wdth=1.0f, bool show_draggers = true);
  ~T3GridViewNode();
};

#else // TA_QT3D

class SoFrame; // 
class SoGroup; // 
class SoMaterial; // 
class SoScrollBar; // 
class T3TransformBoxDragger; //


taTypeDef_Of(T3GridViewNode);

class TA_API T3GridViewNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  TA_SO_NODE_HEADER(T3GridViewNode);
#endif // def __MAKETA__
public:
  static float 		frame_margin; // #HIDDEN #NO_SAVE = .05 size of margin around stage for frame
  static float 		frame_width; // #HIDDEN #NO_SAVE = .02 width of frame itself
  static void		initClass();

  SoFrame*		frame() const {return frame_;} // #IGNORE 
  SoGroup*		grid() const {return grid_;}  // #IGNORE 
  SoMaterial*		matStage() const {return mat_stage_;}  // #IGNORE 
  SoSeparator*		header() const {return header_;}  // #IGNORE 
  SoSeparator*		body() const {return body_;}  // #IGNORE 

  SoScrollBar*		ColScrollBar() const { return col_scroll_bar_; }  // #IGNORE 
  SoScrollBar*		RowScrollBar() const { return row_scroll_bar_; }  // #IGNORE 

  virtual void		render();
  virtual void		setWidth(float wdth);
  virtual float		getWidth() { return width_; }

  //  void		addRemoveChildNode(SoNode* node, bool adding) override {}
  
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

#endif // TA_QT3D

#endif // T3GridViewNode_h
