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

#include <taMisc>

#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>


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

  markerSet_ = NULL;
  marker_sep = NULL;
  marker_size_ = MEDIUM;
  assertMarkerSet();		// just be done with it -- needed for batch updates anyway
  //  initValueColorMode();
  clear();
}

T3GraphLine::~T3GraphLine()
{
  lines = NULL;
  errbars = NULL;
  markerSet_ = NULL;
  textSep_ = NULL;
  textColor_ = NULL;
  labelFont_->unref();
  labelFont_ = NULL;
  complexity_->unref();
  complexity_ = NULL;
}

void T3GraphLine::assertMarkerSet() {
  if (markerSet_) return;
  SoSeparator* ss = this->shapeSeparator(); //cache
  marker_sep = new SoSeparator();
  markerSet_ = new SoMarkerSet(); //
  markerSet_->vertexProperty.setValue(new SoVertexProperty());
  markerSet_->numPoints.setValue(0);
  marker_sep->addChild(markerSet_);
  ss->addChild(marker_sep);
  initValueColorMode();
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

  if (markerSet_) {
    SoMFVec3f& marker_point = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->vertex;
    marker_point.setNum(0);
    markerSet_->markerIndex.setNum(0);
    markerSet_->numPoints.setValue(0);
  }
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
  // called when vcm changes, def color changes, or on clear
  { // lines
    SoVertexProperty* vp = (SoVertexProperty*)lines->vertexProperty.getValue();
    SoSFEnum& mb = vp->materialBinding;
    SoMFUInt32& orderedRGBA = vp->orderedRGBA;

    SoVertexProperty* evp = (SoVertexProperty*)errbars->vertexProperty.getValue();
    SoSFEnum& emb = evp->materialBinding;
    SoMFUInt32& eorderedRGBA = evp->orderedRGBA;

    if (valueColorMode()) {
      mb.setValue(SoVertexProperty::PER_VERTEX);
      orderedRGBA.setNum(0); // must supply colors explicitly
      emb.setValue(SoVertexProperty::PER_VERTEX);
      eorderedRGBA.setNum(0); // must supply colors explicitly
    } else {
      mb.setValue(SoVertexProperty::OVERALL);
      // set one and only color
      orderedRGBA.setNum(1);
      orderedRGBA.set1Value(0, defColor_);

      emb.setValue(SoVertexProperty::OVERALL);
      // set one and only color
      eorderedRGBA.setNum(1);
      eorderedRGBA.set1Value(0, defColor_);
    }
  }
  if (markerSet_) {
    SoVertexProperty* vp = (SoVertexProperty*)markerSet_->vertexProperty.getValue();
    SoSFEnum& mb = vp->materialBinding;
    SoMFUInt32& orderedRGBA = vp->orderedRGBA;
    if (valueColorMode()) {
      mb.setValue(SoVertexProperty::PER_PART); // i.e., per point
      orderedRGBA.setNum(0); // must supply colors explicitly
    }
    else {
      mb.setValue(SoVertexProperty::OVERALL);
      // set one and only color
      orderedRGBA.setNum(1);
      orderedRGBA.set1Value(0, defColor_);
    }
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

  assertMarkerSet();			       // have to assume and pay overhead
  SoMFInt32& marker = markerSet_->markerIndex; // cache
  SoMFVec3f& mpoint = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->vertex;
  marker.enableNotify(false);
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

  SoMFInt32& marker = markerSet_->markerIndex; // cache
  SoMFVec3f& mpoint = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->vertex;
  marker.enableNotify(true);
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

void T3GraphLine::markerAt(const iVec3f& pt, MarkerStyle style) {
  assertMarkerSet();
  SoMFInt32& marker = markerSet_->markerIndex; // cache
  SoMFVec3f& point = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->vertex;

  int mk_idx;
  //note: don't put defaults in these switches, so we can detect missing values
  // also, the NONE may be compiler food...
  switch(marker_size_) {
  case SMALL: {
    switch (style){
    case MarkerStyle_NONE: mk_idx = SoMarkerSet::NONE; break; // compiler food
    case CIRCLE: mk_idx = SoMarkerSet::CIRCLE_LINE_5_5;  break;
    case SQUARE: mk_idx = SoMarkerSet::SQUARE_LINE_5_5;  break;
    case DIAMOND: mk_idx = SoMarkerSet::DIAMOND_LINE_5_5;  break;
    case TRIANGLE: mk_idx = SoMarkerSet::TRIANGLE_LINE_5_5;  break;
    case MINUS: mk_idx = SoMarkerSet::MINUS_5_5;  break;
    case BACKSLASH: mk_idx = SoMarkerSet::BACKSLASH_5_5;  break;
    case BAR: mk_idx = SoMarkerSet::BAR_5_5;  break;
    case SLASH: mk_idx = SoMarkerSet::SLASH_5_5;  break;
    case PLUS: mk_idx = SoMarkerSet::PLUS_5_5;  break;
    case CROSS: mk_idx = SoMarkerSet::CROSS_5_5;  break;
    case STAR: mk_idx = SoMarkerSet::STAR_5_5;  break;
    }
    break;
  }
  case MEDIUM: {
    switch (style){
    case MarkerStyle_NONE: mk_idx = SoMarkerSet::NONE; break; // compiler food
    case CIRCLE: mk_idx = SoMarkerSet::CIRCLE_LINE_7_7;  break;
    case SQUARE: mk_idx = SoMarkerSet::SQUARE_LINE_7_7;  break;
    case DIAMOND: mk_idx = SoMarkerSet::DIAMOND_LINE_7_7;  break;
    case TRIANGLE: mk_idx = SoMarkerSet::TRIANGLE_LINE_7_7;  break;
    case MINUS: mk_idx = SoMarkerSet::MINUS_7_7;  break;
    case BACKSLASH: mk_idx = SoMarkerSet::BACKSLASH_7_7;  break;
    case BAR: mk_idx = SoMarkerSet::BAR_7_7;  break;
    case SLASH: mk_idx = SoMarkerSet::SLASH_7_7;  break;
    case PLUS: mk_idx = SoMarkerSet::PLUS_7_7;  break;
    case CROSS: mk_idx = SoMarkerSet::CROSS_7_7;  break;
    case STAR: mk_idx = SoMarkerSet::STAR_7_7;  break;
    }
    break;
  }
  case LARGE: {
    switch (style){
    case MarkerStyle_NONE: mk_idx = SoMarkerSet::NONE; break; // compiler food
    case CIRCLE: mk_idx = SoMarkerSet::CIRCLE_LINE_9_9;  break;
    case SQUARE: mk_idx = SoMarkerSet::SQUARE_LINE_9_9;  break;
    case DIAMOND: mk_idx = SoMarkerSet::DIAMOND_LINE_9_9;  break;
    case TRIANGLE: mk_idx = SoMarkerSet::TRIANGLE_LINE_9_9;  break;
    case MINUS: mk_idx = SoMarkerSet::MINUS_9_9;  break;
    case BACKSLASH: mk_idx = SoMarkerSet::BACKSLASH_9_9;  break;
    case BAR: mk_idx = SoMarkerSet::BAR_9_9;  break;
    case SLASH: mk_idx = SoMarkerSet::SLASH_9_9;  break;
    case PLUS: mk_idx = SoMarkerSet::PLUS_9_9;  break;
    case CROSS: mk_idx = SoMarkerSet::CROSS_9_9;  break;
    case STAR: mk_idx = SoMarkerSet::STAR_9_9;  break;
    }
    break;
  }
  }

  //note: render count is taken from point count, so add marker first
  marker.set1Value(marker.getNum(), mk_idx);
  point.set1Value(point.getNum(), pt.x, pt.y, -pt.z);
  markerSet_->numPoints.setValue(marker.getNum());
}

void T3GraphLine::markerAt(const iVec3f& pt, MarkerStyle style, const T3Color& c) {
  assertMarkerSet();
  // add the new color -- no way to optimize this, must always have one col per point
  uint32_t new_col = T3Color::makePackedRGBA(c.r, c.g, c.b);
  SoMFUInt32& orderedRGBA = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->orderedRGBA;
  orderedRGBA.set1Value(orderedRGBA.getNum(), new_col);

  markerAt(pt, style);
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

void T3GraphLine::setMarkerSize(MarkerSize sz) {
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

