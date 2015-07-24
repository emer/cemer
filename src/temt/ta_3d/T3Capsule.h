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

#ifndef T3Capsule_h
#define T3Capsule_h 1

// parent includes:
#include <T3ColorEntity>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3Capsule : public T3ColorEntity {
  // a 3D capsule -- cylinder with rounded caps -- useful for limbs etc
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  enum LongAxis {
    LONG_X=1,                   // long axis is in X direction
    LONG_Y,                     // long axis is in Y direction
    LONG_Z,                     // long axis is in Z direction
  };

  Qt3D::QRotateTransform*    axis_rotate;    // extra axis rotation
  LongAxis      axis;
  float         radius;
  float         length;
  
  virtual void  setGeom(LongAxis axis, float radius, float length);
  // set new geometry of cylinder and update
  virtual void  setAxis(LongAxis axis);
  // set new geometry of cylinder and update
  virtual void  setRadius(float radius);
  // set new geometry of cylinder and update
  virtual void  setLength(float length);
  // set new geometry of cylinder and update
  
  T3Capsule(Qt3DNode* parent = 0);
  T3Capsule(Qt3DNode* parent, LongAxis axis, float radius, float length);
  ~T3Capsule();

public slots:
  virtual void  updateGeom(); // update to new geom

protected:
  void init();
};


#endif // T3Capsule_h
