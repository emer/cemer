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



#ifndef DATAGRAPH_SO_H
#define DATAGRAPH_SO_H

#include "t3node_so.h"

/* GRAPHS

  Coordinate reference frames are listed below. This frame is used to
  enable the most natural reference frame for graphs.

  Graph, Line, and Axis objects:
  x - corresponds to Inventor x
  y - corresponds to Inventor y
  z - corresponds to -ve Inventor z, ie., +ve values go into screen

  Graphs object (outer container, standard PDP frame):
  x - Inventor x
  y - -ve Inventor z (i.e., depth of graphs windows, into screen)
  z - Inventor y (height of graphs window)
*/

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

class T3Axis: public T3NodeLeaf { //
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

//  float			axis_length; // #DEF_1 actual length of axis, in display units

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

class T3GraphLine: public T3NodeLeaf { //
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3GraphLine);
#endif // def __MAKETA__
public:
  enum LineStyle { // corresponds to DA_GraphViewSpec::LineStyle
    SOLID,			// -----
    DOT,			// .....
    DASH,			// - - -
    DASH_DOT			// _._._
#ifndef __MAKETA__
    ,LineStyle_MIN = SOLID, // also the default
    LineStyle_MAX = DASH_DOT
#endif
  };

  enum MarkerStyle { // MUST correspond to GraphColSpec::PointStyle
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
//   T3Graph		//
//////////////////////////

class T3Graph: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3Graph);
#endif // def __MAKETA__

public:
  static void		initClass();

  T3Graph(void* dataView_ = NULL);

protected:
//nn  void		setDefaultCaptionTransform(); // override
  ~T3Graph();
};


#endif
