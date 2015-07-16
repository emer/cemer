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

#ifndef T3Entity_h
#define T3Entity_h 1

// parent includes:
#include "ta_def.h"

#ifdef __MAKETA__

class T3Entity; // #IGNORE
class Qt3DNode; // #IGNORE

#else

#include <Qt3DCore>
#include <Qt3DRenderer>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QRotateTransform>

// for maketa:
typedef Qt3D::QNode Qt3DNode; 

class TA_API T3Entity : public Qt3D::QEntity {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Qt3D entity that retains pointers to the standard components for rendering, for convenience, and automatically adds the standard transforms (scale, translate, rotation) in the constructor
  Q_OBJECT
  INHERITED(Qt3D::QEntity)
public:
  Qt3D::QTransform          transform; // overall transform applied to this node -- contains each of the following items:
  Qt3D::QScaleTransform     scale;     // overall scale transform applied to this node
  Qt3D::QTranslateTransform translate; // overall translation transform applied to this node
  Qt3D::QRotateTransform    rotate;    // overall rotation transform applied to this node

  Qt3D::QAbstractMesh*  mesh;      // mesh component for this node
  Qt3D::QMaterial*      material;  // material for this node

  void  addMesh(Qt3D::QAbstractMesh* msh)
  { mesh = msh; addComponent(mesh); }
  // adds mesh component, records the last one added

  void  addMaterial(Qt3D::QMaterial* mat)
  { material = mat; addComponent(material); }
  // adds material component, records the last one added

  T3Entity(Qt3DNode* parent = 0);
  ~T3Entity();
};

#endif  // __MAKETA__

#endif // T3Entity_h
