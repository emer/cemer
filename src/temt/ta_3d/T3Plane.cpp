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

#include "T3Plane.h"

#include <Qt3DRender/QPlaneMesh>
#include <Qt3DRender/QPhongMaterial>

T3Plane::T3Plane(Qt3DNode* parent)
  : inherited(parent)
{
  axis = XZ;
  plane_size.setWidth(1.0f);
  plane_size.setHeight(1.0f);
  init();
}

T3Plane::T3Plane(Qt3DNode* parent, PlaneAxis ax, const QSize& sz)
  : inherited(parent)
{
  axis = ax;
  plane_size = sz;
  init();
}

void T3Plane::init() {
  sub = new T3Entity(this);
  Qt3DRender::QPlaneMesh* cb = new Qt3DRender::QPlaneMesh();
  sub->addMesh(cb);

  sub->addMaterial(phong);
  
  updateSize();
  updateAxis();
}

T3Plane::~T3Plane() {
  
}

void T3Plane::setAxis(PlaneAxis ax) {
  axis = ax;
  updateAxis();
}

void T3Plane::setSize(const QSize& sz) {
  plane_size = sz;
  updateSize();
}

void T3Plane::updateSize() {
  Qt3DRender::QPlaneMesh* cb = dynamic_cast<Qt3DRender::QPlaneMesh*>(sub->mesh);
  cb->setWidth(plane_size.width());
  cb->setHeight(plane_size.height());
}

void T3Plane::updateAxis() {
  switch(axis) {
  case XZ:
    size = QVector3D(plane_size.width(), 1.0e-06f, plane_size.height());
    sub->RotateDeg(1.0f, 0.0f, 0.0f, 0.0f);
    break;
  case XY:
    size = QVector3D(plane_size.width(), plane_size.height(), 1.0e-06f);
    sub->RotateDeg(1.0f, 0.0f, 0.0f, 90.0f);
    break;
  case YZ:
    size = QVector3D(1.0e-06f, plane_size.width(), plane_size.height());
    sub->RotateDeg(0.0f, 0.0f, 1.0f, 90.0f);
    break;
  }
  sub->size = size;             // todo: does sub need color??
}

