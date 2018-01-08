// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "iVec3f.h"

#ifdef TA_QT3D

#include <QVector3D>

iVec3f::iVec3f(const QVector3D& src) {
  x = src.x(); y = src.y(); z = src.z();
}

iVec3f& iVec3f::operator=(const QVector3D& src) {
  x = src.x(); y = src.y(); z = src.z();
  return *this;
}

iVec3f::operator QVector3D() const {
  return QVector3D(x, y, z);
}

#else // TA_QT3D

#include <Inventor/SbLinear.h>

iVec3f::iVec3f(const SbVec3f& src) {
  src.getValue(x, y, z);
}

iVec3f& iVec3f::operator=(const SbVec3f& src) {
  src.getValue(x, y, z);
  return *this;
}

iVec3f::operator SbVec3f() const {
  return SbVec3f(x, y, z);
}

#endif // TA_QT3D
