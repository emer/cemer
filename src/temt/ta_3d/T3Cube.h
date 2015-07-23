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

#ifndef T3Cube_h
#define T3Cube_h 1

// parent includes:
#include <T3Entity>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3Cube : public T3ColorEntity {
  // a 3D cube -- manages a cube mesh
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  virtual void  setSize(const QVector3D& sz);
  // set new size and update
  void  setSize(float xs, float ys, float zs)
  { setSize(QVector3D(xs, ys, zs)); }
  
  T3Cube(Qt3DNode* parent = 0);
  T3Cube(Qt3DNode* parent, const QVector3D& sz);
  ~T3Cube();

public slots:
  virtual void  updateSize(); // update to new size

protected:
  void init();
};

#endif // T3Cube_h
