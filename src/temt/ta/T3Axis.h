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

#ifndef T3Axis_h
#define T3Axis_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:
#include <iVec3f>


// declare all other types mentioned but not required to include:
class SoFont; // 
class iVec3f; // 
class SbRotation; // 
class SoComplexity; // #IGNORE

taTypeDef_Of(T3Axis);

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
  SoFont*		labelFont() const {return labelFont_;} // #IGNORE setup after creating
  float			fontSize() const { return font_size_; }

  void			clear();
  void			addLabel(const char* text, const iVec3f& at);
  // add the label text, justified per axis (used for tick labels)
  void			addLabel(const char* text, const iVec3f& at, int just);
  // add the label text, with explicit justification (used for axis title, and unit labels)
  // SoAsciiText::Justification just
  void			addLabelRot(const char* text, const iVec3f& at, int just,
				    SbRotation& rot);
  // #IGNORE add the label text, with explicit justification (used for axis title, and unit labels)
  void			addLine(const iVec3f& from, const iVec3f to);

  T3Axis(Axis axis = X, T3DataView* dataView_ = NULL, float fnt_sz=.05f, int n_axis = 0);

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

  void			setDefaultCaptionTransform() override; // sets text justif and transform for 3D
  ~T3Axis();
};

#endif // T3Axis_h
