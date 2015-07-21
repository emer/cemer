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

#ifndef T3Sphere_h
#define T3Sphere_h 1

// parent includes:
#include <T3Entity>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3Sphere : public T3ColorEntity {
  // a 3D sphere -- manages a sphere mesh
  Q_OBJECT
  INHERITED(T3ColorEntity)
public:
  float         radius;         // radius of the sphere
  
  virtual void  setRadius(float radius);
  // set new radius and update
  
  T3Sphere(Qt3DNode* parent = 0);
  T3Sphere(Qt3DNode* parent, float radius);
  ~T3Sphere();

public slots:
  virtual void  updateRadius(); // update to new radius

protected:
  void init();
};

#endif // T3Sphere_h
