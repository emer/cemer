// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#include "colorbar_so.h"

#include <Inventor/SbLinear.h>
#include <Inventor/SbName.h>
#include <Inventor/errors/SoDebugError.h>
//#include <Inventor/nodes/SoAsciiText.h>
//#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
//#include <Inventor/nodes/SoEnvironment.h>
//#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNormalBinding.h>
//#include <Inventor/nodes/SoPickStyle.h>
//#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
//#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
//#include <Inventor/nodes/SoUnits.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include "icolor.h"

#include <math.h>

//////////////////////////
//   T3CBar		//
//////////////////////////

SO_NODE_SOURCE(T3CBar);

void T3CBar::initClass()
{
  SO_NODE_INIT_CLASS(T3CBar, T3NodeLeaf, "T3NodeLeaf");
}

T3CBar::T3CBar(ColorScale* c, void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3CBar);

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

T3CBar::~T3CBar(){
  taBase::UnRef(scale);
  scale = NULL;
}

int T3CBar::blocks() {
  int rval = (scale) ? scale->chunks : 1;
  if (rval == 0) rval = 1;  // for robustness
  return rval;
}

void T3CBar::clear() {
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

void T3CBar::SetColorScale(ColorScale* c){
  if (scale == c) return;

  if (scale) {
    clear();
    taBase::UnRef(scale);
  }
  scale = c;
  if (scale) {
    taBase::Ref(scale);
    render();
  }
}

void T3CBar::SetDimensions(float wd, float ht) {
  if ((wd == width) && (height == ht)) return;
  width = wd;
  height = ht;
  render();
}

void T3CBar::render() {
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
    const iColor* col = scale->GetColor(i);
    if (!col) break; // shouldn't happen
    // color
    pcol = T3Color::makePackedRGBA(col->red(), col->green(), col->blue());
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

