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

#include "SoRect.h"

int32_t SoRect_numVtx[1] = {
  6
};

SO_NODE_SOURCE(SoRect);

void SoRect::initClass()
{
  SO_NODE_INIT_CLASS(SoRect, SoTriangleStripSet, "SoTriangleStripSet");
}

SoRect::SoRect() {
  SO_NODE_CONSTRUCTOR(SoRect);

  vertexProperty.setValue(new SoVertexProperty); // note: vp refs/unrefs automatically
  width = height = 1.0f;

  vertex().setNum(6);
  this->numVertices.setValues(0, 1, SoRect_numVtx);
  normal().setNum(1);
  ((SoVertexProperty*)vertexProperty.getValue())->normalBinding.setValue(SoNormalBinding::PER_PART);
  render();
}

SoMFVec3f& SoRect::normal() {
  return ((SoVertexProperty*)vertexProperty.getValue())->normal;
}

void SoRect::render() {
  float w2 = width * 0.5f; // x
  float h2 = height * 0.5f; // y
  SoMFVec3f& p = vertex();
  SoMFVec3f& n = normal();
  int i = 0; // vertex index
  int j = 0; // normal index
  SbVec3f* p_dat = p.startEditing();
  SbVec3f* n_dat = n.startEditing();

  // front
  n_dat[j++].setValue( 0, 0, 1);
  p_dat[i++].setValue(-w2,  h2, 0);
  p_dat[i++].setValue( w2,  h2, 0);
  p_dat[i++].setValue( w2, -h2, 0);
  p_dat[i++].setValue(-w2,  h2, 0);
  p_dat[i++].setValue(-w2, -h2, 0);
  p_dat[i++].setValue( w2, -h2, 0);

  n.finishEditing();
  p.finishEditing();

}

void SoRect::setDimensions(float wd, float ht) {
  if ((width == wd) && (height == ht)) return;
  width = wd;  height = ht;
  render();
}

SoMFVec3f& SoRect::vertex() {
  return ((SoVertexProperty*)vertexProperty.getValue())->vertex;
}

