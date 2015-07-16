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

#include "T3ColorBar.h"
#include <iColor>
#include <T3Color>
#include <math.h>

int T3ColorBar::blocks() {
  int rval = (scale) ? scale->chunks : 1;
  if (rval == 0) rval = 1;  // for robustness
  return rval;
}

#ifdef TA_QT3D

T3ColorBar::T3ColorBar(Qt3DNode* parent, T3DataView* dataView_, ColorScale* c)
  : inherited(parent, dataView_)
{
}

T3ColorBar::~T3ColorBar() {
}

void T3ColorBar::clear() {
  // SoVertexProperty* vp = (SoVertexProperty*)bars_->vertexProperty.getValue(); // cache
  // bars_->numVertices.startEditing();
  // vp->orderedRGBA.startEditing();
  // vp->vertex.startEditing();

  // bars_->numVertices.setNum(0);
  // vp->orderedRGBA.setNum(0);
  // vp->vertex.setNum(0);

  // vp->vertex.finishEditing();
  // vp->orderedRGBA.finishEditing();
  // bars_->numVertices.finishEditing();
  inherited::clear();
}

void T3ColorBar::SetColorScale(ColorScale* c){
  if (scale.ptr() == c) return;

  if ((bool)scale) {
    clear();
  }
  scale = c;
  if ((bool)scale) {
    // render();
  }
}

void T3ColorBar::SetDimensions(float wd, float ht) {
  if ((wd == width) && (height == ht)) return;
  width = wd;
  height = ht;
  // render();
}

// void T3ColorBar::render() {
//   if ((width <= 0.0f) || (height <= 0.0f)) return; // out of bounds
//   if (!scale) return;

  // //note: this routine works whether cleared or not

  // int b = blocks(); // cache
  // float wb = width / b; // width per block
  // float x = -(width / 2.0f); //
  // float h2 = height / 2.0f;
  // uint32_t pcol; // packed color

  // SoVertexProperty* vp = (SoVertexProperty*)bars_->vertexProperty.getValue(); // cache

  // // rect and vertex indices
  // int idx_rct = 0;
  // int idx_vtx = 0;

  // bars_->numVertices.startEditing();
  // vp->orderedRGBA.startEditing();
  // vp->vertex.startEditing();

  // // preset number of items, to avoid unnecessary dynamic enlargements of data structures
  // bars_->numVertices.setNum(b);
  // vp->orderedRGBA.setNum(b);
  // vp->vertex.setNum(b * 4);

  // for (int i = 0; i < b; ++i) { // one strip per color value
  //   bool ok;
  //   iColor col = scale->GetColor(i, &ok);
  //   if (!ok) break; // shouldn't happen
  //   // color
  //   pcol = T3Color::makePackedRGBA(col.red(), col.green(), col.blue());
  //   vp->orderedRGBA.set1Value(idx_rct, pcol);
  //   // num of vertices
  //   bars_->numVertices.set1Value(idx_rct++, 4); // always 4 per face
  //   // a rect, requires 2 faces, 4 coords
  //   vp->vertex.set1Value(idx_vtx++,  x, -h2, 0.0f);
  //   vp->vertex.set1Value(idx_vtx++,  x,  h2, 0.0f);
  //   x += wb;
  //   vp->vertex.set1Value(idx_vtx++,  x, -h2, 0.0f);
  //   vp->vertex.set1Value(idx_vtx++,  x,  h2, 0.0f);
  // }

  // vp->vertex.finishEditing();
  // vp->orderedRGBA.finishEditing();
  // bars_->numVertices.finishEditing();
// }

#else // TA_QT3D

#include <Inventor/SbLinear.h>
#include <Inventor/SbName.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

SO_NODE_SOURCE(T3ColorBar);

void T3ColorBar::initClass()
{
  SO_NODE_INIT_CLASS(T3ColorBar, T3NodeLeaf, "T3NodeLeaf");
}

T3ColorBar::T3ColorBar(ColorScale* c, T3DataView* dataView_)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3ColorBar);

  height = 0.2f;
  width = 1.0f;

  scale = NULL; // set at end

  SoSeparator* ss = this->shapeSeparator(); //cache
  // disable lighting, so colorbar appears correct on both sides
  SoLightModel* lm = new SoLightModel();
  lm->model.setValue(SoLightModel::BASE_COLOR);
  ss->addChild(lm); //note: don't need to access it anymore, so we don't keep its pointer

  bars_ = new SoTriangleStripSet();
  SoVertexProperty* vp = new SoVertexProperty();
  //note: we indicate overall normal binding, but don't supply a normal
  // because we actually don't want the color scale to be black on the back, etc.
//  vp->normal.set1Value(0, 0.0f, 0.0f, 1.0f);
  vp->normalBinding = SoVertexProperty::OVERALL;
  vp->materialBinding = SoVertexProperty::PER_PART; // i.e., rectangle
  bars_->vertexProperty.setValue(vp);

  ss->addChild(bars_);

  SetColorScale(c);
}

T3ColorBar::~T3ColorBar(){
  taBase::UnRef(scale);
  scale = NULL;
}

void T3ColorBar::clear() {
  SoVertexProperty* vp = (SoVertexProperty*)bars_->vertexProperty.getValue(); // cache
  bars_->numVertices.startEditing();
  vp->orderedRGBA.startEditing();
  vp->vertex.startEditing();

  bars_->numVertices.setNum(0);
  vp->orderedRGBA.setNum(0);
  vp->vertex.setNum(0);

  vp->vertex.finishEditing();
  vp->orderedRGBA.finishEditing();
  bars_->numVertices.finishEditing();
  inherited::clear();
}

void T3ColorBar::SetColorScale(ColorScale* c){
  if (scale.ptr() == c) return;

  if ((bool)scale) {
    clear();
  }
  scale = c;
  if ((bool)scale) {
    render();
  }
}

void T3ColorBar::SetDimensions(float wd, float ht) {
  if ((wd == width) && (height == ht)) return;
  width = wd;
  height = ht;
  render();
}

void T3ColorBar::render() {
  if ((width <= 0.0f) || (height <= 0.0f)) return; // out of bounds
  if (!scale) return;

  //note: this routine works whether cleared or not

  int b = blocks(); // cache
  float wb = width / b; // width per block
  float x = -(width / 2.0f); //
  float h2 = height / 2.0f;
  uint32_t pcol; // packed color

  SoVertexProperty* vp = (SoVertexProperty*)bars_->vertexProperty.getValue(); // cache

  // rect and vertex indices
  int idx_rct = 0;
  int idx_vtx = 0;

  bars_->numVertices.startEditing();
  vp->orderedRGBA.startEditing();
  vp->vertex.startEditing();

  // preset number of items, to avoid unnecessary dynamic enlargements of data structures
  bars_->numVertices.setNum(b);
  vp->orderedRGBA.setNum(b);
  vp->vertex.setNum(b * 4);

  for (int i = 0; i < b; ++i) { // one strip per color value
    bool ok;
    iColor col = scale->GetColor(i, &ok);
    if (!ok) break; // shouldn't happen
    // color
    pcol = T3Color::makePackedRGBA(col.red(), col.green(), col.blue());
    vp->orderedRGBA.set1Value(idx_rct, pcol);
    // num of vertices
    bars_->numVertices.set1Value(idx_rct++, 4); // always 4 per face
    // a rect, requires 2 faces, 4 coords
    vp->vertex.set1Value(idx_vtx++,  x, -h2, 0.0f);
    vp->vertex.set1Value(idx_vtx++,  x,  h2, 0.0f);
    x += wb;
    vp->vertex.set1Value(idx_vtx++,  x, -h2, 0.0f);
    vp->vertex.set1Value(idx_vtx++,  x,  h2, 0.0f);
  }

  vp->vertex.finishEditing();
  vp->orderedRGBA.finishEditing();
  bars_->numVertices.finishEditing();
}

#endif // TA_QT3D
