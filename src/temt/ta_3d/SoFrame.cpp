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

#include "SoFrame.h"

int32_t SoFrame_numVtx[10] = { // hor (ver)
  10, // bottom
  4, 4, 4, 4, // outside: l, bk, r, fr
  10, // top
  4, 4, 4, 4 // inside:  l, bk, r, fr
};

SO_NODE_SOURCE(SoFrame);

void SoFrame::initClass()
{
  SO_NODE_INIT_CLASS(SoFrame, SoTriangleStripSet, "SoTriangleStripSet");
}

SoFrame::SoFrame(Orientation ori, float in) {
  SO_NODE_CONSTRUCTOR(SoFrame);

  vertexProperty.setValue(new SoVertexProperty); // note: vp refs/unrefs automatically
  base = height = 1.0f;
  depth = 0.1f;
  inset = in;
  orientation = ori;

  vertex().setNum(56);
  this->numVertices.setValues(0, 10, SoFrame_numVtx);
  normal().setNum(10);
  ((SoVertexProperty*)vertexProperty.getValue())->normalBinding.setValue(SoNormalBinding::PER_PART);
  render();
}

SoMFVec3f& SoFrame::normal() {
  return ((SoVertexProperty*)vertexProperty.getValue())->normal;
}

void SoFrame::render() {
  switch (orientation) {
  case Hor: renderH(); break;
  case Ver: renderV(); break;
  }
}

void SoFrame::renderH() {
  float w2 = base / 2.0f; // x
  float d2 = depth / 2.0f; // y
  float h2 = height / 2.0f; // z
  SoMFVec3f& p = vertex();
  SoMFVec3f& n = normal();
  int i = 0; // vertex index
  int j = 0; // normal index
  SbVec3f* p_dat = p.startEditing();
  SbVec3f* n_dat = n.startEditing();

  // bottom
  n_dat[j++].setValue( 0, -1, 0);
  p_dat[i++].setValue(-w2,-d2, h2);
  p_dat[i++].setValue(-(w2-inset),-d2, (h2-inset));
  p_dat[i++].setValue( w2,-d2, h2);
  p_dat[i++].setValue( (w2-inset),-d2, (h2-inset));
  p_dat[i++].setValue( w2,-d2,-h2);
  p_dat[i++].setValue( (w2-inset),-d2,-(h2-inset));
  p_dat[i++].setValue(-w2,-d2,-h2);
  p_dat[i++].setValue(-(w2-inset),-d2,-(h2-inset));
  p_dat[i++].setValue(-w2,-d2, h2);
  p_dat[i++].setValue(-(w2-inset),-d2, (h2-inset));

  //outside: l
  n_dat[j++].setValue( -1, 0, 0);
  p_dat[i++].setValue(-w2, d2,-h2);
  p_dat[i++].setValue(-w2,-d2,-h2);
  p_dat[i++].setValue(-w2, d2, h2);
  p_dat[i++].setValue(-w2,-d2, h2);

  //outside: bk
  n_dat[j++].setValue( 0, 0, -1);
  p_dat[i++].setValue( w2,-d2,-h2);
  p_dat[i++].setValue(-w2,-d2,-h2);
  p_dat[i++].setValue( w2, d2,-h2);
  p_dat[i++].setValue(-w2, d2,-h2);

  //outside: r
  n_dat[j++].setValue( 1, 0, 0);
  p_dat[i++].setValue( w2,-d2,-h2);
  p_dat[i++].setValue( w2, d2,-h2);
  p_dat[i++].setValue( w2,-d2, h2);
  p_dat[i++].setValue( w2, d2, h2);

  //outside: fr
  n_dat[j++].setValue( 0, 0, 1);
  p_dat[i++].setValue( w2,-d2, h2);
  p_dat[i++].setValue(-w2,-d2, h2);
  p_dat[i++].setValue( w2, d2, h2);
  p_dat[i++].setValue(-w2, d2, h2);

  // top
  n_dat[j++].setValue( 0, 1, 0);
  p_dat[i++].setValue(-w2, d2, h2);
  p_dat[i++].setValue(-(w2-inset), d2, h2-inset);
  p_dat[i++].setValue(-w2, d2,-h2);
  p_dat[i++].setValue(-(w2-inset), d2,-(h2-inset));
  p_dat[i++].setValue( w2, d2,-h2);
  p_dat[i++].setValue( (w2-inset), d2,-(h2-inset));
  p_dat[i++].setValue( w2, d2, h2);
  p_dat[i++].setValue( (w2-inset), d2, (h2-inset));
  p_dat[i++].setValue(-w2, d2, h2);
  p_dat[i++].setValue(-(w2-inset), d2, h2-inset);
  // inside
  w2 -= inset;
  h2 -= inset;
  //inside: l
  n_dat[j++].setValue( 1, 0, 0);
  p_dat[i++].setValue(-w2, d2,-h2);
  p_dat[i++].setValue(-w2,-d2,-h2);
  p_dat[i++].setValue(-w2, d2, h2);
  p_dat[i++].setValue(-w2,-d2, h2);

  //inside: bk
  n_dat[j++].setValue( 0, 0, 1);
  p_dat[i++].setValue( w2,-d2,-h2);
  p_dat[i++].setValue(-w2,-d2,-h2);
  p_dat[i++].setValue( w2, d2,-h2);
  p_dat[i++].setValue(-w2, d2,-h2);

  //inside: r
  n_dat[j++].setValue( -1, 0, 0);
  p_dat[i++].setValue( w2,-d2,-h2);
  p_dat[i++].setValue( w2, d2,-h2);
  p_dat[i++].setValue( w2,-d2, h2);
  p_dat[i++].setValue( w2, d2, h2);

  //inside: fr
  n_dat[j++].setValue( 0, 0, -1);
  p_dat[i++].setValue( w2,-d2, h2);
  p_dat[i++].setValue(-w2,-d2, h2);
  p_dat[i++].setValue( w2, d2, h2);
  p_dat[i++].setValue(-w2, d2, h2);

  n.finishEditing();
  p.finishEditing();
}

void SoFrame::renderV() {
  float w2 = base / 2.0f; // x (same as H)
  float d2 = depth / 2.0f; // z
  float h2 = height / 2.0f; // y
  SoMFVec3f& p = vertex();
  SoMFVec3f& n = normal();
  int i = 0; // vertex index
  int j = 0; // normal index
  SbVec3f* p_dat = p.startEditing();
  SbVec3f* n_dat = n.startEditing();
  // back
  n_dat[j++].setValue( 0, 0, -1);
  p_dat[i++].setValue(-w2, h2, -d2);
  p_dat[i++].setValue(-(w2-inset), (h2-inset), -d2);
  p_dat[i++].setValue( w2, h2, -d2);
  p_dat[i++].setValue( (w2-inset), (h2-inset), -d2);
  p_dat[i++].setValue( w2,-h2, -d2);
  p_dat[i++].setValue( (w2-inset),-(h2-inset), -d2);
  p_dat[i++].setValue(-w2,-h2, -d2);
  p_dat[i++].setValue(-(w2-inset),-(h2-inset), -d2);
  p_dat[i++].setValue(-w2, h2, -d2);
  p_dat[i++].setValue(-(w2-inset),(h2-inset), -d2);

  //outside: l
  n_dat[j++].setValue( -1, 0, 0);
  p_dat[i++].setValue(-w2,-h2,-d2);
  p_dat[i++].setValue(-w2, h2,-d2);
  p_dat[i++].setValue(-w2,-h2, d2);
  p_dat[i++].setValue(-w2, h2, d2);

  //outside: tp
  n_dat[j++].setValue( 0, 1, 0);
  p_dat[i++].setValue(-w2, h2,-d2);
  p_dat[i++].setValue( w2, h2,-d2);
  p_dat[i++].setValue(-w2, h2, d2);
  p_dat[i++].setValue( w2, h2, d2);

  //outside: r
  n_dat[j++].setValue( 1, 0, 0);
  p_dat[i++].setValue( w2, h2,-d2);
  p_dat[i++].setValue( w2,-h2,-d2);
  p_dat[i++].setValue( w2, h2, d2);
  p_dat[i++].setValue( w2,-h2, d2);

  //outside: bt
  n_dat[j++].setValue( 0, -1, 0);
  p_dat[i++].setValue( w2,-h2,-d2);
  p_dat[i++].setValue(-w2,-h2,-d2);
  p_dat[i++].setValue( w2,-h2, d2);
  p_dat[i++].setValue(-w2,-h2, d2);

  // front
  n_dat[j++].setValue( 0, 0, 1);
  p_dat[i++].setValue(-w2, h2, d2);
  p_dat[i++].setValue(-(w2-inset), (h2-inset), d2);
  p_dat[i++].setValue( w2, h2, d2);
  p_dat[i++].setValue( (w2-inset), (h2-inset), d2);
  p_dat[i++].setValue( w2,-h2, d2);
  p_dat[i++].setValue( (w2-inset),-(h2-inset), d2);
  p_dat[i++].setValue(-w2,-h2, d2);
  p_dat[i++].setValue(-(w2-inset),-(h2-inset), d2);
  p_dat[i++].setValue(-w2, h2, d2);
  p_dat[i++].setValue(-(w2-inset),(h2-inset), d2);

  w2 -= inset;
  h2 -= inset;
  //inside: l
  n_dat[j++].setValue( 1, 0, 0);
  p_dat[i++].setValue(-w2,-h2,-d2);
  p_dat[i++].setValue(-w2, h2,-d2);
  p_dat[i++].setValue(-w2,-h2, d2);
  p_dat[i++].setValue(-w2, h2, d2);

  //inside: tp
  n_dat[j++].setValue( 0, -1, 0);
  p_dat[i++].setValue(-w2, h2,-d2);
  p_dat[i++].setValue( w2, h2,-d2);
  p_dat[i++].setValue(-w2, h2, d2);
  p_dat[i++].setValue( w2, h2, d2);

  //inside: r
  n_dat[j++].setValue( -1, 0, 0);
  p_dat[i++].setValue( w2, h2,-d2);
  p_dat[i++].setValue( w2,-h2,-d2);
  p_dat[i++].setValue( w2, h2, d2);
  p_dat[i++].setValue( w2,-h2, d2);

  //inside: bt
  n_dat[j++].setValue( 0, 1, 0);
  p_dat[i++].setValue( w2,-h2,-d2);
  p_dat[i++].setValue(-w2,-h2,-d2);
  p_dat[i++].setValue( w2,-h2, d2);
  p_dat[i++].setValue(-w2,-h2, d2);

  n.finishEditing();
  p.finishEditing();

}

void SoFrame::setDimensions(float bs, float ht, float dp) {
  if ((base == bs) && (height == ht) && (depth == dp)) return;
  base = bs;  height = ht;  depth = dp;
  render();
}

void SoFrame::setDimensions(float bs, float ht, float dp, float in) {
  if ((base == bs) && (height == ht) && (depth == dp) && (inset == in)) return;
  base = bs;  height = ht;  depth = dp;  inset = in;
  render();
}

void SoFrame::setInset(float value) {
  if (inset == value) return;
  inset = value;
  render();
}

void SoFrame::setOrientation(Orientation ori) {
  if (orientation == ori) return;
  orientation = ori;
  render();
}

SoMFVec3f& SoFrame::vertex() {
  return ((SoVertexProperty*)vertexProperty.getValue())->vertex;
}

