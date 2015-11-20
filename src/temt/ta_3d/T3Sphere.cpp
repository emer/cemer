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

#include "T3Sphere.h"

#include <Qt3DRender/QSphereMesh>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;

T3Sphere::T3Sphere(Qt3DNode* parent)
  : inherited(parent)
{
  radius = 1.0f;
  init();
}

T3Sphere::T3Sphere(Qt3DNode* parent, float rad)
  : inherited(parent)
{
  radius = rad;
  init();
}

void T3Sphere::init() {
  QSphereMesh* cb = new QSphereMesh();
  addMesh(cb);
  updateRadius();
}

T3Sphere::~T3Sphere() {
  
}

void T3Sphere::setRadius(float rad) {
  radius = rad;
  updateRadius();
}

void T3Sphere::updateRadius() {
  size = QVector3D(radius, radius, radius);
  QSphereMesh* cb = dynamic_cast<QSphereMesh*>(mesh);
  cb->setRadius(radius);
}

