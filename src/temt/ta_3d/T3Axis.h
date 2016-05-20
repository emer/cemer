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
class iVec3f; // 

#ifdef TA_QT3D

#include <QColor>

class T3LineStrip;

class TA_API T3Axis: public T3NodeLeaf {
  // dataview is the GraphAxisView
  Q_OBJECT
  INHERITED(T3NodeLeaf)
public:
  enum Axis {
    X,
    Y,
    Z
  };

  Axis			axis;
  float                 font_size;
  int			axis_n;	// number of axis (can be multiple Y axes..)
  float                 width;
  QColor                color;

  T3Entity*             labels;
  T3LineStrip*          lines;
  
  void			clear() override;
  void			addLabel(const char* text, const iVec3f& at);
  // add the label text, justified per axis (used for tick labels)
  void			addLabel(const char* text, const iVec3f& at, int just);
  // add the label text, with explicit justification (used for axis title, and unit labels)
  void			addLabelRot(const char* text, const iVec3f& at, int just,
				    const QVector3D& rot_ax, float rot_ang);
  // #IGNORE add the label text, with explicit justification (used for axis title, and unit labels)
  void			addLine(const iVec3f& from, const iVec3f to);

  void                  setNodeUpdating(bool updating) override;
  void			setDefaultCaptionTransform() override; // sets text justif and transform for 3D

  T3Axis(Qt3DNode* par = NULL, T3DataView* dataView_ = NULL, Axis axis = X, 
         float fnt_sz=.05f, float width = 1.0f, int n_axis = 0);
  ~T3Axis();

protected:
  int                   n_labels; // number of active labels
};


#else // TA_QT3D


class SoFont; // 
class SbRotation; // 
class SoComplexity; // #IGNORE

taTypeDef_Of(T3Axis);

class TA_API T3Axis: public T3NodeLeaf {
  // dataview is the GraphAxisView
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  TA_SO_NODE_HEADER(T3Axis);
#endif // def __MAKETA__
public:
  enum Axis {
    X,
    Y,
    Z
  };

  static void		initClass(); //

  float			font_size;
  
  Axis			axis() const {return axis_;} //note: lifetime invariant
  SoFont*		labelFont() const {return labelFont_;} // #IGNORE setup after creating
  float			fontSize() const { return font_size; }

  void			clear() override;
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
  SoSeparator*		labels;

  void			setDefaultCaptionTransform() override; // sets text justif and transform for 3D
  ~T3Axis();
};

#endif // TA_QT3D


#endif // T3Axis_h
