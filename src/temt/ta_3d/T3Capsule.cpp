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

#include "T3Capsule.h"

#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QPhongMaterial>


T3Capsule::T3Capsule(Qt3DNode* parent)
  : inherited(parent)
{
  size = QVector3D(1.0f, 1.0f, 1.0f);
  init();
}

T3Capsule::T3Capsule(Qt3DNode* parent, const QVector3D& sz)
  : inherited(parent)
{
  size = sz;
  init();
}

void T3Capsule::init() {
  Qt3D::QCylinderMesh* cb = new Qt3D::QCylinderMesh();
  addMesh(cb);
  // cb->setXExtent(size.x());
  // cb->setYExtent(size.y());
  // cb->setZExtent(size.z());

  Qt3D::QPhongMaterial* mt = new Qt3D::QPhongMaterial();
  mt->setAmbient(color);
  mt->setDiffuse(color);
  addMaterial(mt);
}

T3Capsule::~T3Capsule() {
  
}

void T3Capsule::setSize(const QVector3D& sz) {
  size = sz;
  updateSize();
}

void T3Capsule::updateSize() {
  Qt3D::QCylinderMesh* cb = dynamic_cast<Qt3D::QCylinderMesh*>(mesh);
  // cb->setXExtent(size.x());
  // cb->setYExtent(size.y());
  // cb->setZExtent(size.z());
}

void T3Capsule::setColor(const QColor& clr) {
  color = clr;
  updateColor();
}

void T3Capsule::updateColor() {
  Qt3D::QPhongMaterial* mt = dynamic_cast<Qt3D::QPhongMaterial*>(material);
  if(mt) {
    mt->setAmbient(color);
    mt->setDiffuse(color);
  }
}


