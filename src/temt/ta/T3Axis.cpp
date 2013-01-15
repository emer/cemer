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

#include "T3Axis.h"
#include <taString>
#include <T3Misc>
#include <taMath_float>
#include <iVec3f>


#include <taMisc>

#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

SO_NODE_SOURCE(T3Axis);

void T3Axis::initClass()
{
  SO_NODE_INIT_CLASS(T3Axis, T3NodeLeaf, "T3NodeLeaf");
}

T3Axis::T3Axis(Axis ax, T3DataView* dataView_, float fnt_sz, int n_axis)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3Axis);
//  axis_length = 1.0f; // dummy/default
  axis_ = ax;
  n_ax_ = n_axis;
  font_size_ = fnt_sz;

  SoSeparator* ss = this->shapeSeparator(); //cache

  lines = new SoLineSet();
  lines->vertexProperty.setValue(new SoVertexProperty());
  ss->addChild(lines);

  labels = new SoSeparator();
  ss->addChild(labels);

  complexity_ = new SoComplexity;
  complexity_->ref(); // we re-add to the labels group every clear
  complexity_->value.setValue(taMisc::text_complexity);

  labelFont_ = new SoFont();
  labelFont_->ref(); // we re-add to the labels group every clear
  labelFont_->name = "Arial";
  labelFont_->size.setValue(font_size_);

  clear(); // initializes everything correctly
}

T3Axis::~T3Axis()
{
  complexity_->unref();
  complexity_ = NULL;
  labelFont_->unref();
  labelFont_ = NULL;
  labels = NULL;
  lines = NULL;
}

void T3Axis::addLabel(const char* text, const iVec3f& at) {
  SoAsciiText::Justification just;
  switch (axis_) {
  case X: just = SoAsciiText::CENTER;  break;
  case Y: {
    if(n_ax_ == 0)
      just = SoAsciiText::RIGHT;
    else
      just = SoAsciiText::LEFT;
    break;
  }
  case Z: just = SoAsciiText::RIGHT;  break;
  }
  addLabel(text, at, just);
}

void T3Axis::addLabel(const char* text, const iVec3f& at, int just) {
  //note: we assume (for simplicity) that each new label is at a different place
  SoTranslation* tr = new SoTranslation();
  tr->translation.setValue(at.x - last_label_at.x, at.y - last_label_at.y,
			   -(at.z - last_label_at.z));
  last_label_at = at;
  labels->addChild(tr);

  // render the actual text
  SoAsciiText* txt = new SoAsciiText();
  txt->justification.setValue((SoAsciiText::Justification)just);
  txt->string.setValue(text);
  labels->addChild(txt);
}

void T3Axis::addLabelRot(const char* text, const iVec3f& at, int just, SbRotation& rot) {
  //note: we assume (for simplicity) that each new label is at a different place
  SoTranslation* tr = new SoTranslation();
  tr->translation.setValue(at.x - last_label_at.x, at.y - last_label_at.y,
			   -(at.z - last_label_at.z));
  last_label_at = at;
  labels->addChild(tr);

  // render the actual text
  SoSeparator* sep = new SoSeparator;
  SoTransform* tx = new SoTransform;
  tx->rotation.setValue(rot);
  sep->addChild(tx);
  SoAsciiText* txt = new SoAsciiText();
  txt->justification.setValue((SoAsciiText::Justification)just);
  txt->string.setValue(text);
  sep->addChild(txt);
  labels->addChild(sep);
}

void T3Axis::addLine(const iVec3f& from, const iVec3f to) {
  lines->numVertices.startEditing();
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  point.startEditing();

  // add the two new line vertices
  int pt_idx = point.getNum();
  point.set1Value(pt_idx++, from.x, from.y, -from.z);
  point.set1Value(pt_idx++, to.x, to.y, -to.z);

  // add num of vertices (2) of this new line
  lines->numVertices.set1Value(lines->numVertices.getNum(), 2);

  point.finishEditing();
  lines->numVertices.finishEditing();
}

void T3Axis::clear() {
  lines->numVertices.setNum(0);
  SoMFVec3f& lines_point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  lines_point.setNum(0);

  last_label_at = 0.0f;
  labels->removeAllChildren();
  labels->addChild(complexity_);
  labels->addChild(labelFont_);
  inherited::clear();
}

void T3Axis::setDefaultCaptionTransform() {
  //note: this is the one for 3d objects -- 2d replace this
  captionNode_->justification = SoAsciiText::CENTER;
  transformCaption(SbVec3f(0.0f, 0.1f, 0.45f));
}

/*void T3Axis::setPicked(bool value) {
  SoSeparator* ss = shapeSeparator(); //cache
  SoDrawStyle* ds = (SoDrawStyle*)getNodeByName(ss, "drawStyle");
  if (value) { // picking
    if (ds) return; // already there
    ds = new SoDrawStyle();
    ds->setName("drawStyle");
    ds->style.setValue(SoDrawStyle::LINES);
    insertChildAfter(ss, ds, material());
  } else { // unpicking
    if (ds)
      ss->removeChild(ds);
  }
} */

