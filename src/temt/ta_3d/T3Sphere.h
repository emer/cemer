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
#include <QColor>

// declare all other types mentioned but not required to include:

class TA_API T3Sphere : public T3Entity {
  // a 3D cube -- manages a cube mesh, and adds a Phong Material for rendering of given color
  Q_OBJECT
  INHERITED(T3Entity)
public:
  QColor        color;          // color -- applies to all color types
    
  void  setSize(const QVector3D& sz);
  // set new size and update
  virtual void  setColor(const QColor& color);
  // set the color and update display
  
  T3Sphere(Qt3DNode* parent = 0);
  T3Sphere(Qt3DNode* parent, const QVector3D& sz);
  ~T3Sphere();

public slots:
  virtual void  updateSize(); // update to new size
  virtual void  updateColor(); // update to new color

protected:
  void init();
};

#endif // T3Sphere_h
