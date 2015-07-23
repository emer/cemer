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

T3Capsule::T3Capsule(Qt3DNode* parent)
  : inherited(parent)
{
  axis = LONG_Z;
  radius = 1.0f;
  length = 1.0f;
  init();
}

T3Capsule::T3Capsule(Qt3DNode* parent, LongAxis ax, float rad, float len)
  : inherited(parent)
{
  axis = ax;
  radius = rad;
  length = len;
  init();
}

void T3Capsule::init() {
  // todo: will color automatically inherit down to sub-entity??  should..
  sub = new T3Entity(this);
  // todo: update to capsule!
  Qt3D::QCylinderMesh* cb = new Qt3D::QCylinderMesh();
  sub->addMesh(cb);
  updateGeom();
}

T3Capsule::~T3Capsule() {
  
}

void T3Capsule::setGeom(LongAxis ax, float rad, float len) {
  axis = ax;
  radius = rad;
  length = len;
  updateGeom();
}

void T3Capsule::setAxis(LongAxis ax) {
  axis = ax;
  updateGeom();
}

void T3Capsule::setRadius(float rad) {
  radius = rad;
  updateGeom();
}

void T3Capsule::setLength(float len) {
  length = len;
  updateGeom();
}

void T3Capsule::updateGeom() {
  Qt3D::QCylinderMesh* cb = dynamic_cast<Qt3D::QCylinderMesh*>(sub->mesh);
  cb->setRadius(radius);
  cb->setLength(length);
  switch(axis) {
  case LONG_X:
    size = QVector3D(length, 2.0f*radius, 2.0f*radius);
    sub->RotateDeg(0.0f, 1.0f, 0.0f, -90.0f);
    break;
  case LONG_Y:
    size = QVector3D(2.0f*radius, length, 2.0f*radius);
    sub->RotateDeg(1.0f, 0.0f, 0.0f, -90.0f);
    break;
  case LONG_Z:
    size = QVector3D(2.0f*radius, 2.0f*radius, length);
    sub->RotateDeg(1.0f, 0.0f, 0.0f, 0.0f);
    break;
  }    
  sub->size = size;             // todo: does sub need color??
}

