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

#ifndef T3AnnotationView_h
#define T3AnnotationView_h 1

// parent includes:
#include <T3DataView>

// member includes:

// declare all other types mentioned but not required to include:
class T3Annotation; //

taTypeDef_Of(T3AnnotationView);

class TA_API T3AnnotationView : public T3DataView {
  // gui viewer of a T3Annotation item
INHERITED(T3DataView)
public:
  T3Annotation*         Anno() const { return (T3Annotation*)data();}
  virtual void          SetAnno(T3Annotation* ob);

  void  SetDefaultName() {} // leave it blank
  TA_BASEFUNS_NOCOPY(T3AnnotationView);
protected:
  void  Initialize();
  void  Destroy();

  override void         Render_pre();
  override void         Render_impl();
};

#endif // T3AnnotationView_h
