// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#ifndef TA_DATATABLE_SO_H
#define TA_DATATABLE_SO_H

#include "ta_def.h"
#include "t3node_so.h"

// externals
class SbVec3f; // #IGNORE
class SoAction; // #IGNORE
class SoBaseColor; // #IGNORE
class SoCube; // #IGNORE
class SoFont; // #IGNORE
class SoCalculator; // #IGNORE
class SoTransformBoxDragger; // #IGNORE

// forwards
class T3GridViewNode;
class T3GraphViewNode; //

//////////////////////////
//   T3GridViewNode	//
//////////////////////////

/* 3d grid views
  legend: *-existing; +-new this class

      *transform: SoTransform (for translating to pos)
      +stage: SoSeparator -- actual area of the grid
        +mat_stage: SoMaterial -- for setting the font color 
        +txlt_stage: SoTranslation -- sets origin to upper-left of stage
        +head: SoSeparator -- for header objects
        +body: SoSeparator -- for body objects
	+grid: SoSeparator -- for grid lines
      *captionSeparator: SoSeparator  
            shapeSeparator: SoSeparator
      *txfm_shape: SoTransform
      *material: SoMaterial
      +frame: SoFrame
*/

class TA_API T3GridViewNode: public T3NodeLeaf {
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3GridViewNode);
#endif // def __MAKETA__
public:
  static float 		drag_size; // = .04 size of dragger control object
  static float 		frame_margin; // = .05 size of margin around stage for frame
  static float 		frame_width; // = .02 width of frame itself
  static void		initClass();

  SoFrame*		frame() const {return frame_;} 
  SoGroup*		grid() const {return grid_;}
  SoMaterial*		matStage() const {return mat_stage_;}
  SoSeparator*		header() const {return header_;}
  SoSeparator*		body() const {return body_;}

  virtual void		render();
  virtual void		setWidth(float wdth);
  virtual float		getWidth() { return width_; }

  T3GridViewNode(void* dataView_ = NULL, float wdth=1.0f, bool show_draggers = true);
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
  SoSeparator*		drag_sep_;
  SoTransform*		drag_xf_;
  SoTransformBoxDragger* dragger_;
  SoCalculator*		drag_trans_calc_;

  ~T3GridViewNode();
};


class TA_API T3GridColViewNode: public T3NodeLeaf {
  // grid view column -- mainly just for editing column spec ifno
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3GridColViewNode);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3GridColViewNode(void* colspec = NULL);

protected:
  ~T3GridColViewNode();
};


///////////////////////////////////////////////////////////////////////////
// 		Graphing

//////////////////////////
//   T3Axis		//
//////////////////////////

/*
    this: SoSeparator - the object itself
      transform: SoTransform
      material: SoMaterial
      [drawStyle: SoDrawStyle] (inserted when picked, to make wireframe instead of solid)
    shapeSeparator(): note: same as this
      points: SoCoordinate3;
      lines: SoLineSet;

    for addLabel, the 'at' reference point is as follows:
    X - vtop hcenter, text centered
    Y - vmid hright, text right just
    Z - TBD

*/

class TA_API T3Axis: public T3NodeLeaf {
  // dataview is the GraphAxisView
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3Axis);
#endif // def __MAKETA__
public:
  enum Axis {
    X,
    Y,
    Z
  };

  static void		initClass(); //
  Axis			axis() const {return axis_;} //note: lifetime invariant
  SoFont*		labelFont() const {return labelFont_;} // setup after creating

  void			clear();
  void			addLabel(const char* text, const iVec3f& at);
  // add the label text, justified per axis (used for tick labels)
  void			addLabel(const char* text, const iVec3f& at, int just);
  // add the label text, with explicit justification (used for axis title, and unit labels)
  // SoAsciiText::Justification just
  void			addLine(const iVec3f& from, const iVec3f to);

  T3Axis(Axis axis = X, void* dataView_ = NULL);
protected:
  Axis		axis_; //note: lifetime invariant
  SoSeparator* 		line_sep;
  SoDrawStyle* 		line_style;
  SoLineSet*		lines; // we use the vertexProperty for points etc.
  iVec3f 		last_label_at; // used so we just need to issue delta translates
  SoFont*		labelFont_;
  SoSeparator*		labels;

  void			setDefaultCaptionTransform(); // override, sets text justif and transform for 3D
  ~T3Axis();
};


//////////////////////////
//   T3GraphLine	//
//////////////////////////

/*
    this: SoSeparator - the object itself
      transform: SoTransform
      material: SoMaterial
      [drawStyle: SoDrawStyle] (inserted when picked, to make wireframe instead of solid)
      shape: SoCylinder (could be changed to something else)

    Color drawing is in one of two modes:
      valueColorMode=false -- all lines and points are drawn in a single color
      valueColorMode=true -- all lines and points are drawn in a specified color;

*/

class TA_API T3GraphLine: public T3NodeLeaf {
  // a graph line -- data is GraphColView
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3GraphLine);
#endif // def __MAKETA__
public:
  enum LineStyle { // corresponds to GraphColView::LineStyle
    SOLID,			// -----
    DOT,			// .....
    DASH,			// - - -
    DASH_DOT			// _._._
#ifndef __MAKETA__
    ,LineStyle_MIN = SOLID, // also the default
    LineStyle_MAX = DASH_DOT
#endif
  };

  enum MarkerStyle { // MUST correspond to GraphColView::PointStyle
    CIRCLE = 1,			// o
    SQUARE,			// []
    DIAMOND,			// <>
    TRIANGLE,			//
    MINUS,			// -
    BACKSLASH,		//
    BAR,			// |
    SLASH,			// /
    PLUS,			// +
    CROSS,			// x
    STAR			// *
#ifndef __MAKETA__
    ,MarkerStyle_NONE = 0, // pseudo value, not actually used
    MarkerStyle_MIN = CIRCLE,
    MarkerStyle_MAX = STAR
#endif
  };

  static void		initClass();

  SoFont*		labelFont() const {return labelFont_;} // setup after creating
  //nn  LineStyle		lineStyle() {return lineStyle_;}
  void 			setLineStyle(LineStyle value, float line_width = 0.0f);
  bool			valueColorMode(){return valueColorMode_;}
  void			setValueColorMode(bool value);

  void			clear(); //

  // text api
  void			textAt(const iVec3f& pt, const char* str);
  // render indicated text at the pt

  // non valueColor drawing api
  void			setDefaultColor(const T3Color& color); // call after reset to set default color (black if not called)
  void			moveTo(const iVec3f& pt); // use to start a new line segment
  void			lineTo(const iVec3f& to); // add arc to current line, in default color
  void			markerAt(const iVec3f& pt, MarkerStyle style); // render a marker at indicated location in default color

  // valueColor drawing api
  void			moveTo(const iVec3f& pt, const T3Color& color);
  // use to start a new line segment in valueColor mode
  void			lineTo(const iVec3f& to, const T3Color& color);
  //  add arc to current line, in valueColor mode (optimizes if color same as last arc)
  void			markerAt(const iVec3f& pt, MarkerStyle style, const T3Color& color);
  // render a marker at indicated location in valueColor mode

  T3GraphLine(void* dataView_ = NULL);

protected:
  uint32_t		defColor_; // def is black
  bool			valueColorMode_;
  LineStyle		lineStyle_;
  SoSeparator*		line_sep;
  SoDrawStyle*		lineDrawStyle_;
  SoLineSet*		lines; // we use the vertexProperty for points etc.
  SoMarkerSet*		markerSet_; // created if needed; we use the vertexProperty for points etc.
  SoSeparator*		textSep_; // optional text separator
  SoFont*		labelFont_;
  SoPackedColor*	textColor_;
  iVec3f		lastText_; // where last text was rendered, for our next translate

  void			assertMarkerSet();
    // makes sure marketset is created, and initialized base on modes
  void			assertText();
    // makes sure text separator is created;
  void			initValueColorMode(); // called in several places
  void			setDefaultCaptionTransform(); // override, sets text justif and transform for 3D
  ~T3GraphLine();
};


//////////////////////////
//   T3GraphViewNode	//
//////////////////////////

class TA_API T3GraphViewNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3GraphViewNode);
#endif // def __MAKETA__
public:
  static float 		drag_size; // = .04 size of dragger control object
  static float 		frame_margin; // = .05 size of margin around stage for frame
  static float 		frame_width; // = .02 width of frame itself
  static void		initClass();

  T3GraphViewNode(void* dataView_ = NULL, float wdth=1.0f, bool show_draggers = true);
   // dataview is a GraphTableView object

  virtual void		render();
  virtual void		setWidth(float wdth);
  virtual float		getWidth() { return width_; }

  virtual SoSeparator*	x_axis() { return x_axis_sep_; }
  virtual SoSeparator*	z_axis() { return z_axis_sep_; }
  virtual SoSeparator*	y_axes() { return y_axes_sep_; }
  virtual SoSeparator*	lines() { return lines_sep_; }

protected:
  float			width_;
  SoFrame*		frame_; 

  // in childNodes:
  SoMaterial*		chld_mat_; // first item in childNodes -- default material
  SoSeparator*		x_axis_sep_;
  SoSeparator*		z_axis_sep_;
  SoSeparator*		y_axes_sep_; // can be multiple

  SoSeparator*		lines_sep_;  // where the lines go (at 0,0)

  bool			show_drag_;
  SoSeparator*		drag_sep_;
  SoTransform*		drag_xf_;
  SoTransformBoxDragger* dragger_;
  SoCalculator*		drag_trans_calc_;

  ~T3GraphViewNode();
};


#endif

