// Copyright, 1995-2005, Regents of the University of Colorado,
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
class SoComplexity; // #IGNORE 

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
      *shapeSeparator: SoSeparator
        *txfm_shape: SoTransform
        *material: SoMaterial
        +frame: SoFrame
      + scroll_sep
        +scroll_xf
*/

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
  T3TransformBoxDragger* drag_;	// my position dragger

  SoScrollBar*		col_scroll_bar_;
  SoTransform*		col_sb_tx_; // col scrollbar transform
  SoScrollBar*		row_scroll_bar_;

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
  float			fontSize() const { return font_size_; }

  void			clear();
  void			addLabel(const char* text, const iVec3f& at);
  // add the label text, justified per axis (used for tick labels)
  void			addLabel(const char* text, const iVec3f& at, int just);
  // add the label text, with explicit justification (used for axis title, and unit labels)
  // SoAsciiText::Justification just
  void			addLabelRot(const char* text, const iVec3f& at, int just,
				    SbRotation& rot);
  // add the label text, with explicit justification (used for axis title, and unit labels)
  void			addLine(const iVec3f& from, const iVec3f to);

  T3Axis(Axis axis = X, void* dataView_ = NULL, float fnt_sz=.05f, int n_axis = 0);
protected:
  Axis			axis_; // note: lifetime invariant
  int			n_ax_;	// number of axis (can be multiple Y axes..)
  SoSeparator* 		line_sep;
  SoDrawStyle* 		line_style;
  SoLineSet*		lines; // we use the vertexProperty for points etc.
  iVec3f 		last_label_at; // used so we just need to issue delta translates
  SoComplexity*		complexity_;
  SoFont*		labelFont_;
  float			font_size_;
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

  enum MarkerSize {
    SMALL,
    MEDIUM,
    LARGE,
  };

  static void		initClass();

  SoFont*		labelFont() const {return labelFont_;} // setup after creating
  void 			setLineStyle(LineStyle value, float line_width = 0.0f);
  void			setMarkerSize(MarkerSize sz);
  bool			valueColorMode(){return valueColorMode_;}
  void			setValueColorMode(bool value);

  void			clear(); //

  // text api
  void			textAt(const iVec3f& pt, const char* str);
  // render indicated text at the pt

  void			startBatch();
  // start a batch edit of the lines -- must be followed by a finishBatch call!  just turns off notification of updates
  void			finishBatch();
  // done with batch update -- turn notification back on and do a touch

  // non valueColor drawing api
  void			setDefaultColor(const T3Color& color); // call after reset to set default color (black if not called)
  void			moveTo(const iVec3f& pt); // use to start a new line segment
  void			lineTo(const iVec3f& to); // add arc to current line, in default color
  void			errBar(const iVec3f& pt, float err, float bar_width); // render error bar at given point
  void			markerAt(const iVec3f& pt, MarkerStyle style); // render a marker at indicated location in default color

  // valueColor drawing api
  void			moveTo(const iVec3f& pt, const T3Color& color);
  // use to start a new line segment in valueColor mode
  void			lineTo(const iVec3f& to, const T3Color& color);
  //  add arc to current line, in valueColor mode
  void			errBar(const iVec3f& pt, float err, float bar_width, const T3Color& color);
  // render error bar at given point
  void			markerAt(const iVec3f& pt, MarkerStyle style, const T3Color& color);
  // render a marker at indicated location in valueColor mode

  T3GraphLine(void* dataView_ = NULL, float fnt_sz = .05f);

protected:
  uint32_t		defColor_; // def is black
  bool			valueColorMode_;
  LineStyle		lineStyle_;
  MarkerSize		marker_size_;
  SoSeparator*		line_sep;
  SoDrawStyle*		lineDrawStyle_;
  SoLineSet*		lines;		// we use the vertexProperty for points etc.
  SoLineSet*		errbars;
  SoMarkerSet*		markerSet_; // created if needed; we use the vertexProperty for points etc.
  SoSeparator*		textSep_; // optional text separator
  SoComplexity*		complexity_;
  SoFont*		labelFont_;
  float			font_size_;
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
  virtual SoSeparator*	legend() { return legend_sep_; }
  virtual SoSeparator*	graphs() { return graphs_sep_; }

  SoScrollBar*		RowScrollBar() const { return row_scroll_bar_; }

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


#endif

