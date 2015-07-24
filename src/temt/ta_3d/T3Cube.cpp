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

#include "T3Cube.h"

#include <Qt3DRenderer/QCuboidMesh>


T3Cube::T3Cube(Qt3DNode* parent)
  : inherited(parent)
{
  size = QVector3D(1.0f, 1.0f, 1.0f);
  init();
}

T3Cube::T3Cube(Qt3DNode* parent, const QVector3D& sz)
  : inherited(parent)
{
  size = sz;
  init();
}

void T3Cube::init() {
  Qt3D::QCuboidMesh* cb = new Qt3D::QCuboidMesh();
  addMesh(cb);
  updateSize();
}

T3Cube::~T3Cube() {
  
}

void T3Cube::setSize(const QVector3D& sz) {
  const float min_size = 1.0e-6f;
  size = sz;
  size.setX(MAX(min_size, size.x()));
  size.setY(MAX(min_size, size.y()));
  size.setZ(MAX(min_size, size.z()));
  updateSize();
}

void T3Cube::updateSize() {
  Qt3D::QCuboidMesh* cb = dynamic_cast<Qt3D::QCuboidMesh*>(mesh);
  cb->setXExtent(size.x());
  cb->setYExtent(size.y());
  cb->setZExtent(size.z());
}

