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

#include "T3Cylinder.h"

#include <Qt3DRender/QCylinderMesh>
#include <Qt3DRender/QPhongMaterial>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;

T3Cylinder::T3Cylinder(Qt3DNode* parent)
  : inherited(parent)
{
  axis = LONG_Z;
  radius = 1.0f;
  length = 1.0f;
  init();
}

T3Cylinder::T3Cylinder(Qt3DNode* parent, LongAxis ax, float rad, float len)
  : inherited(parent)
{
  axis = ax;
  radius = rad;
  length = len;
  init();
}

void T3Cylinder::init() {
  axis_rotate = new QRotateTransform;
  transform->addTransform(axis_rotate);
  QCylinderMesh* cb = new QCylinderMesh();
  addMesh(cb);
  updateGeom();
}

T3Cylinder::~T3Cylinder() {
  
}

void T3Cylinder::setGeom(LongAxis ax, float rad, float len) {
  axis = ax;
  radius = rad;
  length = len;
  updateGeom();
}

void T3Cylinder::setAxis(LongAxis ax) {
  axis = ax;
  updateGeom();
}

void T3Cylinder::setRadius(float rad) {
  radius = rad;
  updateGeom();
}

void T3Cylinder::setLength(float len) {
  length = len;
  updateGeom();
}

void T3Cylinder::updateGeom() {
  QCylinderMesh* cb = dynamic_cast<QCylinderMesh*>(mesh);
  cb->setRadius(radius);
  cb->setLength(length);
  switch(axis) {
  case LONG_X:
    size = QVector3D(length, 2.0f*radius, 2.0f*radius);
    axis_rotate->setAxis(QVector3D(0.0f, 0.0f, 1.0f));
    axis_rotate->setAngleDeg(-90.0f);
    break;
  case LONG_Y:
    size = QVector3D(2.0f*radius, length, 2.0f*radius);
    axis_rotate->setAxis(QVector3D(1.0f, 0.0f, 0.0f));
    axis_rotate->setAngleDeg(0.0f);
    break;
  case LONG_Z:
    size = QVector3D(2.0f*radius, 2.0f*radius, length);
    axis_rotate->setAxis(QVector3D(1.0f, 0.0f, 0.0f));
    axis_rotate->setAngleDeg(-90.0f);
    break;
  }    
}

