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

#include "T3GraphLine.h"
#include <taString>
#include <T3Misc>
#include <taMath_float>
#include <T3Color>
#include <taSvg>

#include <taMisc>

#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

float T3GraphLine::mark_pts[160] = {
  // CIRCLE = 0,17
  1, 0,
  0.92388, 0.382683,
  0.707107, 0.707107,
  0.382683, 0.92388,
  0, 1,
  -0.382683, 0.92388,
  -0.707107, 0.707107,
  -0.92388, 0.382683,
  -1, 0,
  -0.92388, -0.382683,
  -0.707107, -0.707107,
  -0.382683, -0.92388,
  0, -1,
  0.382683, -0.92388,
  0.707107, -0.707107,
  0.92388, -0.382683,
  1, 0,
  // SQUARE = 17,5
  -1,-1,
  -1,1,
  1,1,
  1,-1,
  -1,-1,
  // DIAMOND = 22,5
  -1,0,
  0,1,
  1,0,
  0,-1,
  -1,0,
  // TRIANGLE_UP = 27,4
  -1,-1,
  0,1,
  1,-1,
  -1,-1,
  // TRIANGLE_DN = 31,4
  -1,1,
  0,-1,
  1,1,
  -1,1,
  // TRIANGLE_RT = 35,4
  -1,1,
  1,0,
  -1,-1,
  -1,1,
  // TRIANGLE_LT = 39,4
  1,1,
  -1,0,
  1,-1,
  1,1,
  // POINT = 43,5
  -.1,-.1,
  -.1,.1,
  .1,.1,
  .1,-.1,
  -.1,-.1,
  // PLUS = 48,5
  -1,0,
  1,0,
  0,0,
  0,-1,
  0,1,
  // CROSS = 53,5
  -1,1,
  1,-1,
  0,0,
  1,1,
  -1,-1,
  // STAR = 58,11  (plus + cross)
  -1,0,
  1,0,
  0,0,
  0,-1,
  0,1,
  0,0,
  -1,1,
  1,-1,
  0,0,
  1,1,
  -1,-1
};

int T3GraphLine::mark_pts_n = 70;

// first one below is a null -- CIRCLE starts at 1
int T3GraphLine::mark_start[16] = {
  0, 0, 17, 22, 27, 31, 35, 39, 43, 48, 53, 58
};

int T3GraphLine::mark_n[16] = {
  0, 17, 5, 5, 4, 4, 4, 4, 5, 5, 5, 11
};

SO_NODE_SOURCE(T3GraphLine);

void T3GraphLine::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphLine, T3NodeLeaf, "T3NodeLeaf");
}

T3GraphLine::T3GraphLine(T3DataView* dataView_, float fnt_sz)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphLine);

  font_size_ = fnt_sz;
  textSep_ = NULL;
  textColor_ = NULL;
  lastText_ = 0.0f;

  complexity_ = new SoComplexity;
  complexity_->ref(); // we re-add to the labels group every clear
  complexity_->value.setValue(taMisc::text_complexity);

  labelFont_ = new SoFont();
  labelFont_->ref(); // we re-add to the text group after clear
  labelFont_->name = "Arial";
  labelFont_->size.setValue(font_size_);

  defColor_ = 0xff; // black, opaque
  valueColorMode_ = false;
  SoSeparator* ss = this->shapeSeparator(); //cache
  line_sep = new SoSeparator();
  lineDrawStyle_ = new SoDrawStyle();
  line_sep->addChild(lineDrawStyle_);
  ss->addChild(line_sep);
  //note:  just let linestyle/width be the defaults

  lines = new SoLineSet();
  lines->vertexProperty.setValue(new SoVertexProperty());
  line_sep->addChild(lines);

  errbars = new SoLineSet();
  errbars->vertexProperty.setValue(new SoVertexProperty());
  line_sep->addChild(errbars);

  markers = new SoLineSet();
  markers->vertexProperty.setValue(new SoVertexProperty());
  line_sep->addChild(markers);

  marker_size_ = .04f;
  initValueColorMode();
  clear();
}

T3GraphLine::~T3GraphLine()
{
  lines = NULL;
  errbars = NULL;
  markers = NULL;
  textSep_ = NULL;
  textColor_ = NULL;
  labelFont_->unref();
  labelFont_ = NULL;
  complexity_->unref();
  complexity_ = NULL;
}

void T3GraphLine::assertText() {
  if (textSep_) return;
  textSep_ = new SoSeparator();
  textSep_->addChild(complexity_);
  textSep_->addChild(labelFont_);

  textColor_ = new SoPackedColor();
  textColor_->orderedRGBA.setValue(defColor_);
  textSep_->addChild(textColor_);
  shapeSeparator()->addChild(textSep_);
}

void T3GraphLine::clear() {
  lines->numVertices.setNum(0);
  SoMFVec3f& lines_point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  lines_point.setNum(0);

  errbars->numVertices.setNum(0);
  SoMFVec3f& errbars_point = ((SoVertexProperty*)errbars->vertexProperty.getValue())->vertex;
  errbars_point.setNum(0);

  markers->numVertices.setNum(0);
  SoMFVec3f& markers_point = ((SoVertexProperty*)markers->vertexProperty.getValue())->vertex;
  markers_point.setNum(0);

  // easiest for text is just to nuke
  if (textSep_) {
    shapeSeparator()->removeChild(textSep_); // unrefs, and thus nukes
    textSep_ = NULL;
    textColor_ = NULL;
    lastText_ = 0.0f;
  }
  inherited::clear();
}

void T3GraphLine::initValueColorMode() {
  // called when vcm changes, def color changes, or on clear lines
  SoVertexProperty* vp = (SoVertexProperty*)lines->vertexProperty.getValue();
  SoSFEnum& mb = vp->materialBinding;
  SoMFUInt32& orderedRGBA = vp->orderedRGBA;

  SoVertexProperty* evp = (SoVertexProperty*)errbars->vertexProperty.getValue();
  SoSFEnum& emb = evp->materialBinding;
  SoMFUInt32& eorderedRGBA = evp->orderedRGBA;

  SoVertexProperty* mvp = (SoVertexProperty*)markers->vertexProperty.getValue();
  SoSFEnum& mmb = mvp->materialBinding;
  SoMFUInt32& morderedRGBA = mvp->orderedRGBA;

  if (valueColorMode()) {
    mb.setValue(SoVertexProperty::PER_VERTEX);
    orderedRGBA.setNum(0); // must supply colors explicitly
    emb.setValue(SoVertexProperty::PER_VERTEX);
    eorderedRGBA.setNum(0); // must supply colors explicitly
    mmb.setValue(SoVertexProperty::PER_VERTEX);
    morderedRGBA.setNum(0); // must supply colors explicitly
  }
  else {
    mb.setValue(SoVertexProperty::OVERALL);
    // set one and only color
    orderedRGBA.setNum(1);
    orderedRGBA.set1Value(0, defColor_);

    emb.setValue(SoVertexProperty::OVERALL);
    // set one and only color
    eorderedRGBA.setNum(1);
    eorderedRGBA.set1Value(0, defColor_);

    mmb.setValue(SoVertexProperty::OVERALL);
    // set one and only color
    morderedRGBA.setNum(1);
    morderedRGBA.set1Value(0, defColor_);
  }
}

void T3GraphLine::startBatch() {
  SoMFInt32& nv = lines->numVertices;
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  nv.enableNotify(false);
  point.enableNotify(false);

  SoMFInt32& env = errbars->numVertices;
  SoMFVec3f& epoint = ((SoVertexProperty*)errbars->vertexProperty.getValue())->vertex;
  env.enableNotify(false);
  epoint.enableNotify(false);

  SoMFInt32& mnv = markers->numVertices;
  SoMFVec3f& mpoint = ((SoVertexProperty*)markers->vertexProperty.getValue())->vertex;
  mnv.enableNotify(false);
  mpoint.enableNotify(false);
}

void T3GraphLine::finishBatch() {
  SoMFInt32& nv = lines->numVertices;
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  nv.enableNotify(true);
  point.enableNotify(true);

  SoMFInt32& env = errbars->numVertices;
  SoMFVec3f& epoint = ((SoVertexProperty*)errbars->vertexProperty.getValue())->vertex;
  env.enableNotify(true);
  epoint.enableNotify(true);

  SoMFInt32& mnv = markers->numVertices;
  SoMFVec3f& mpoint = ((SoVertexProperty*)markers->vertexProperty.getValue())->vertex;
  mnv.enableNotify(true);
  mpoint.enableNotify(true);

//   point.touch();
//   mpoint.touch();
}

void T3GraphLine::moveTo(const iVec3f& pt) {
  SoMFInt32& nv = lines->numVertices; // cache
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;

  // add the new line vertex
  point.set1Value(point.getNum(), pt.x, pt.y, -pt.z);
  // now, add a new slot update # pts in list
  nv.set1Value(nv.getNum(), 1);
}

void T3GraphLine::lineTo(const iVec3f& to) {
  SoMFInt32& nv = lines->numVertices; // cache
  int idx = nv.getNum() - 1; // cache
  //must have a line going...
  if (idx < 0) {
    //TODO: output error msg
    return;
  }
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  // add the new line vertex
  point.set1Value(point.getNum(), to.x, to.y, -to.z);
  // now, update # pts in list
  nv.set1Value(idx, nv[idx] + 1);
}

void T3GraphLine::moveTo(const iVec3f& pt, const T3Color& c) {
  uint32_t new_col = T3Color::makePackedRGBA(c.r, c.g, c.b);
  // always add the new color, then add the point
  SoMFUInt32& orderedRGBA = ((SoVertexProperty*)lines->vertexProperty.getValue())->orderedRGBA;
  orderedRGBA.set1Value(orderedRGBA.getNum(), new_col);
  moveTo(pt);
}

void T3GraphLine::lineTo(const iVec3f& to, const T3Color& c) {
  uint32_t new_col = T3Color::makePackedRGBA(c.r, c.g, c.b);
  SoMFUInt32& orderedRGBA = ((SoVertexProperty*)lines->vertexProperty.getValue())->orderedRGBA;
  orderedRGBA.set1Value(orderedRGBA.getNum(), new_col);
  lineTo(to);
}

void T3GraphLine::errBar(const iVec3f& pt, float err, float bwd) {
  SoMFInt32& env = errbars->numVertices; // cache
  SoMFVec3f& epoint = ((SoVertexProperty*)errbars->vertexProperty.getValue())->vertex;

  int lidx = env.getNum();

  // lower bar
  env.set1Value(lidx, 2);
  epoint.set1Value(epoint.getNum(), pt.x-bwd, pt.y-err, -pt.z);
  epoint.set1Value(epoint.getNum(), pt.x+bwd, pt.y-err, -pt.z);

  // upper bar
  env.set1Value(lidx+1, 2);
  epoint.set1Value(epoint.getNum(), pt.x-bwd, pt.y+err, -pt.z);
  epoint.set1Value(epoint.getNum(), pt.x+bwd, pt.y+err, -pt.z);

  // vertical bar
  env.set1Value(lidx+2, 2);
  epoint.set1Value(epoint.getNum(), pt.x, pt.y-err, -pt.z);
  epoint.set1Value(epoint.getNum(), pt.x, pt.y+err, -pt.z);
}

void T3GraphLine::errBar(const iVec3f& pt, float err, float bwd, const T3Color& c) {
  uint32_t new_col = T3Color::makePackedRGBA(c.r, c.g, c.b);

  uint32_t vals[6];
  for(int i=0;i<6;i++) vals[i] = new_col;

  // always add the new color, then add the point
  SoMFUInt32& eorderedRGBA = ((SoVertexProperty*)errbars->vertexProperty.getValue())->orderedRGBA;

  int lidx = eorderedRGBA.getNum();
  eorderedRGBA.setValues(lidx, 6, vals);
  errBar(pt, err, bwd);
}

void T3GraphLine::markerAt(const iVec3f& pt, MarkerStyle style, const T3Color& c) {
  if(style < MarkerStyle_MIN) style = MarkerStyle_MIN;
  if(style > MarkerStyle_MAX) style = MarkerStyle_MAX;

  uint32_t new_col = T3Color::makePackedRGBA(c.r, c.g, c.b);

  uint32_t vals[17];            // worst case
  for(int i=0;i<17;i++) vals[i] = new_col;

  SoMFUInt32& orderedRGBA = ((SoVertexProperty*)markers->vertexProperty.getValue())->orderedRGBA;
  int lidx = orderedRGBA.getNum();

  orderedRGBA.setValues(lidx, mark_n[style], vals);

  markerAt(pt, style);
}

void T3GraphLine::markerAt(const iVec3f& pt, MarkerStyle style) {
  if(style < MarkerStyle_MIN) style = MarkerStyle_MIN;
  if(style > MarkerStyle_MAX) style = MarkerStyle_MAX;

  SoMFInt32& mnv = markers->numVertices; // cache
  SoMFVec3f& mpoint = ((SoVertexProperty*)markers->vertexProperty.getValue())->vertex;

  int n = mark_n[style];
  int st = mark_start[style];

  int lidx = mnv.getNum();
  mnv.set1Value(lidx,n);

  for(int i = 0; i< n; i++) {
    float mx = mark_pt(st+i,X);
    float my = mark_pt(st+i,Y);
    mpoint.set1Value(mpoint.getNum(), pt.x + mx, pt.y + my, -pt.z);
  }
}

String T3GraphLine::markerAtSvg(const iVec3f& pt, MarkerStyle style) {
  if(style < MarkerStyle_MIN) style = MarkerStyle_MIN;
  if(style > MarkerStyle_MAX) style = MarkerStyle_MAX;

  String rval;
  int n = mark_n[style];
  int st = mark_start[style];

  for(int i = 0; i< n; i++) {
    float mx = mark_pt(st+i,X);
    float my = mark_pt(st+i,Y);
    
    if(i == 0)
      rval << "M ";
    else 
      rval << "L ";
    rval << taSvg::Coords(pt.x + mx, pt.y + my, pt.z);
  }
  return rval;
}


void T3GraphLine::setDefaultCaptionTransform() {
  //note: this is the one for 3d objects -- 2d replace this
  captionNode_->justification = SoAsciiText::CENTER;
  transformCaption(SbVec3f(0.0f, 0.1f, 0.45f));
}

/*void T3GraphLine::setPicked(bool value) {
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

void T3GraphLine::setDefaultColor(const T3Color& c) {
  uint32_t tmp_col = T3Color::makePackedRGBA(c.r, c.g, c.b);
  if (defColor_ == tmp_col) return;
  defColor_ = tmp_col;
  initValueColorMode();
  if (textColor_) {
    textColor_->orderedRGBA.setValue(defColor_);
  }

}

void T3GraphLine::setLineStyle(LineStyle value, float line_width) {
  unsigned short pattern;
  switch (value) {
  case SOLID:
    pattern = 0xffff; // 1111111111111111
    break;
  case DOT:
    pattern = 0x3030; // 0011000000110000
    break;
  case DASH:
    pattern = 0x0f0f; // 0000111100001111
    break;
  case DASH_DOT:
    pattern = 0x060f; // 0000011000001111
    break;
  }
  lineDrawStyle_->linePattern.setValue(pattern);
  lineDrawStyle_->lineWidth.setValue(line_width);
}

void T3GraphLine::setMarkerSize(float sz) {
  marker_size_ = sz;
}

void T3GraphLine::setValueColorMode(bool value) {
  if (valueColorMode_ == value) return;
  valueColorMode_ = value;
  initValueColorMode();
}

void T3GraphLine::textAt(const iVec3f& pt, const char* str) {
  assertText();
  SoTranslation* tr = new SoTranslation();
  tr->translation.setValue(
    pt.x - lastText_.x,
    pt.y - lastText_.y,
    -(pt.z - lastText_.z));
  textSep_->addChild(tr);
  SoAsciiText* at = new SoAsciiText();
  at->string.setValue(str);
  textSep_->addChild(at);
  lastText_ = pt;
}

