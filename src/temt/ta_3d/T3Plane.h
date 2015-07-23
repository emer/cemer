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

#ifndef T3Plane_h
#define T3Plane_h 1

// parent includes:
#include <T3ColorEntity>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3Plane : public T3ColorEntity {
  // a 2D plane, oriented in the specified axes with specified plane_size -- updates the corresponding values of the size vector
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  enum PlaneAxis {              // what axis to orient the plane in
    YZ=1,                       // x is norm, y is width, z is height
    XZ,                         // y is norm, x is width, z is height
    XY,                         // z is norm, x is width, y is height
  };

  T3Entity*     sub;            // plane lives in sub-entity to encapsulate axis rotation
  PlaneAxis     axis;           // axis along which plane is oriented -- default is XZ
  QSize         plane_size;     // size of the plane -- this then updates size according to axis (null axis has an infinitessimal size of 1.0e-6)

  virtual void  setAxis(PlaneAxis axis);
  // set a new axis and update
  virtual void  setSize(const QSize& sz);
  // set new size and update
  inline void  setSize(float wd, float ht)
  { setSize(QSize(wd, ht)); }
  // set new size and update
  
  T3Plane(Qt3DNode* parent = 0);
  T3Plane(Qt3DNode* parent, PlaneAxis axis, const QSize& sz);
  ~T3Plane();

public slots:
  virtual void  updateAxis(); // update to new axis
  virtual void  updateSize(); // update to new size

protected:
  void init();
};

#endif // T3Plane_h
