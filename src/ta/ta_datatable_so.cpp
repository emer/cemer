// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.


#include "ta_datatable_so.h"

//incl the coin header to get gl.h #include <GL/gl.h>
#include <Inventor/system/gl.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoPackedColor.h>
//#include <Inventor/nodes/SoPerspectiveCamera.h>
//#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCalculator.h>

#include <math.h>
#include <limits.h>

//////////////////////////
//   T3GridViewNode	//
//////////////////////////

float T3GridViewNode::drag_size = .04f;
float T3GridViewNode::frame_margin = .05f;
float T3GridViewNode::frame_width = .02f;

extern void T3GridViewNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3GridViewNode);

void T3GridViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridViewNode, T3NodeLeaf, "T3NodeLeaf");
}

T3GridViewNode::T3GridViewNode(void* dataView_, float wdth, bool show_draggers)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GridViewNode);

  width_ = wdth;
  show_drag_ = show_draggers;

  if(show_drag_) {
    drag_sep_ = new SoSeparator;
    drag_xf_ = new SoTransform;
    drag_xf_->scaleFactor.setValue(drag_size, drag_size, drag_size);
    drag_xf_->translation.setValue(-frame_margin, 0.0f, 0.0f);
    drag_sep_->addChild(drag_xf_);
    dragger_ = new SoTransformBoxDragger;
    drag_sep_->addChild(dragger_);

    // super-size me so stuff is actually grabable!
    dragger_->setPart("scaler.scaler", new SoBigScaleUniformScaler(.6f));
    dragger_->setPart("rotator1.rotator", new SoBigTransformBoxRotatorRotator(.4f));
    dragger_->setPart("rotator2.rotator", new SoBigTransformBoxRotatorRotator(.4f));
    dragger_->setPart("rotator3.rotator", new SoBigTransformBoxRotatorRotator(.4f));

    drag_trans_calc_ = new SoCalculator;
    drag_trans_calc_->ref();
    drag_trans_calc_->A.connectFrom(&dragger_->translation);

    // expr set in render below
    //   String expr = "oA = vec3f(" + String(.5f * (width_ + 2.0f * frame_margin)) + " + " + String(drag_size)
    //     + " * A[0], .5 + " +  String(drag_size)
    //     + " * A[1], " + String(drag_size) + " * A[2])";

    //   drag_trans_calc_->expression = expr.chars();

    txfm_shape()->translation.connectFrom(&drag_trans_calc_->oA);
    txfm_shape()->rotation.connectFrom(&dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&dragger_->scaleFactor);

    dragger_->addFinishCallback(T3GridViewNode_DragFinishCB, (void*)this);
  }

  stage_ = new SoSeparator;
  // stage_->addChild(drag_sep_);
  mat_stage_ = new SoMaterial;
  mat_stage_->diffuseColor.setValue(0, 0, 0); // black
  stage_->addChild(mat_stage_);
  txlt_stage_ = new SoTranslation;
  stage_->addChild(txlt_stage_);
  header_ = new SoSeparator;
  stage_->addChild(header_);
  body_ = new SoSeparator;
  stage_->addChild(body_);
  grid_ = new SoSeparator;
  stage_->addChild(grid_);
  insertChildAfter(topSeparator(), stage_, transform());

  if(show_drag_) {
    insertChildAfter(topSeparator(), drag_sep_, transform());
  }
  
  SoMaterial* mat = material(); //cache
  mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // blue/green
  mat->transparency.setValue(0.5f);

  SoSeparator* ss = shapeSeparator(); // cache
  frame_ = new SoFrame(SoFrame::Ver);
  insertChildAfter(ss, frame_, material());
}

T3GridViewNode::~T3GridViewNode()
{
  stage_ = NULL;
  mat_stage_ = NULL;
  txlt_stage_ = NULL;
  header_ = NULL;
  body_ = NULL;
  frame_ = NULL;
  grid_ = NULL;
}

void T3GridViewNode::setWidth(float wdth) {
  width_ = wdth;
  render();
}

void T3GridViewNode::render() {
  float frmg2 = 2.0f * frame_margin;

  if(show_drag_) {
    String expr = "oA = vec3f(" + String(.5f * width_) + " + " + String(drag_size)
      + " * A[0], " + String(.5f * (1.0f + frmg2)) + " + " +  String(drag_size)
      + " * A[1], " + String(drag_size) + " * A[2])";

    drag_trans_calc_->expression = expr.chars();
  }

  txlt_stage_->translation.setValue(0.0f, 1.0f + frame_margin, 0.0f);
  frame_->setDimensions(width_ + frmg2, 1.0f + frmg2, frame_width, frame_width);
  txfm_shape()->translation.setValue(.5f * width_, .5f * (1.0f + frmg2), 0.0f);
  // note: also change in DragFinishCB in qtso
  SoFont* font = captionFont(true);
  transformCaption(iVec3f(0.1f, -((float)font->size.getValue()), 0.0f)); // move caption below the frame
}

//////////////////////////
//   T3GridColViewNode	//
//////////////////////////

// this guy is managed by the _qtso and is only for selecting the column spec!

SO_NODE_SOURCE(T3GridColViewNode);

void T3GridColViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridColViewNode, T3NodeLeaf, "T3NodeLeaf");
}

T3GridColViewNode::T3GridColViewNode(void* colspec)
:inherited(colspec)
{
  SO_NODE_CONSTRUCTOR(T3GridColViewNode);
}

T3GridColViewNode::~T3GridColViewNode()
{
  
}



//////////////////////////////////
//	  T3Axis		//
//////////////////////////////////


SO_NODE_SOURCE(T3Axis);

void T3Axis::initClass()
{
  SO_NODE_INIT_CLASS(T3Axis, T3NodeLeaf, "T3NodeLeaf");
}

T3Axis::T3Axis(Axis ax, void* dataView_, float fnt_sz, int n_axis)
:inherited(dataView_)
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

  labelFont_ = new SoFont();
  labelFont_->ref(); // we re-add to the labels group every clear
  labelFont_->size.setValue(font_size_);

  clear(); // initializes everything correctly
}

T3Axis::~T3Axis()
{
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
  tr->translation.setValue(
    at.x - last_label_at.x,
    at.y - last_label_at.y,
    -(at.z - last_label_at.z));
  last_label_at = at;
  labels->addChild(tr);

  // render the actual text
  SoAsciiText* txt = new SoAsciiText();
  txt->justification.setValue((SoAsciiText::Justification)just);
  txt->string.setValue(text);
  labels->addChild(txt);
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


//////////////////////////////////
//	  T3GraphLine		//
//////////////////////////////////


SO_NODE_SOURCE(T3GraphLine);

void T3GraphLine::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphLine, T3NodeLeaf, "T3NodeLeaf");
}

T3GraphLine::T3GraphLine(void* dataView_, float fnt_sz)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphLine);

  font_size_ = fnt_sz;
  textSep_ = NULL;
  textColor_ = NULL;
  labelFont_ = new SoFont();
  labelFont_->ref(); // we re-add to the text group after clear
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

  markerSet_ = NULL;
  assertMarkerSet();		// just be done with it..
//   initValueColorMode();
}

T3GraphLine::~T3GraphLine()
{
  lines = NULL;
  markerSet_ = NULL;
  textSep_ = NULL;
  textColor_ = NULL;
  labelFont_->unref();
  labelFont_ = NULL;
}

void T3GraphLine::assertMarkerSet() {
  if (markerSet_) return;
  SoSeparator* ss = this->shapeSeparator(); //cache
  markerSet_ = new SoMarkerSet(); //
  markerSet_->vertexProperty.setValue(new SoVertexProperty());
  ss->addChild(markerSet_);
  initValueColorMode();
}

void T3GraphLine::assertText() {
  if (textSep_) return;
  textSep_ = new SoSeparator();
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
  if (markerSet_) {
    SoMFVec3f& marker_point = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->vertex;
    marker_point.setNum(0);
    //note: weird bug happens if we go back to no markers -- so we put a dummy NONE instead
    markerSet_->markerIndex.set1Value(0, SoMarkerSet::NONE);
    markerSet_->markerIndex.setNum(1);
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
    if (valueColorMode()) {
      mb.setValue(SoVertexProperty::PER_VERTEX);
      orderedRGBA.setNum(0); // must supply colors explicitly
    } else {
      mb.setValue(SoVertexProperty::OVERALL);
      // set one and only color
      orderedRGBA.setNum(1);
      orderedRGBA.set1Value(0, defColor_);
    }
  }
  if (markerSet_) {
    SoVertexProperty* vp = (SoVertexProperty*)markerSet_->vertexProperty.getValue();
    SoSFEnum& mb = vp->materialBinding;
    SoMFUInt32& orderedRGBA = vp->orderedRGBA;
    if (valueColorMode()) {
      mb.setValue(SoVertexProperty::PER_PART); // i.e., per point
      orderedRGBA.setNum(0); // must supply colors explicitly
    } else {
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
  SoMFInt32& marker = markerSet_->markerIndex; // cache
  SoMFVec3f& mpoint = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->vertex;
  marker.enableNotify(true);
  mpoint.enableNotify(true);

  point.touch();
  mpoint.touch();
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

void T3GraphLine::markerAt(const iVec3f& pt, MarkerStyle style) {
  assertMarkerSet();
  SoMFInt32& marker = markerSet_->markerIndex; // cache
  SoMFVec3f& point = ((SoVertexProperty*)markerSet_->vertexProperty.getValue())->vertex;

  int mk_idx;
  switch (style){
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

  //note: render count is taken from point count, so add marker first
  //note: we clear by leaving a NONE marker in pos 1, so detect that and overwrite if needed
  if ((marker.getNum() == 1) && (marker[0] == SoMarkerSet::NONE))
    marker.set1Value(0, mk_idx);
  else
    marker.set1Value(marker.getNum(), mk_idx);
  point.set1Value(point.getNum(), pt.x, pt.y, -pt.z);

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
    pattern = 0x3333; // 0011001100110011
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

//////////////////////////
//   T3GraphViewNode	//
//////////////////////////

float T3GraphViewNode::drag_size = .04f;
float T3GraphViewNode::frame_margin = .2f;
float T3GraphViewNode::frame_width = .02f;

extern void T3GraphViewNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3GraphViewNode);

void T3GraphViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphViewNode, T3NodeParent, "T3NodeParent");
}

T3GraphViewNode::T3GraphViewNode(void* dataView_, float wdth, bool show_draggers)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphViewNode);

  width_ = wdth;
  show_drag_ = show_draggers;

  if(show_drag_) {
    drag_sep_ = new SoSeparator;
    drag_xf_ = new SoTransform;
    drag_xf_->scaleFactor.setValue(drag_size, drag_size, drag_size);
    drag_xf_->translation.setValue(-frame_margin, -frame_margin, 0.0f);
    drag_sep_->addChild(drag_xf_);
    dragger_ = new SoTransformBoxDragger;
    drag_sep_->addChild(dragger_);
    topSeparator()->addChild(drag_sep_);

    // super-size me so stuff is actually grabable!
    dragger_->setPart("scaler.scaler", new SoBigScaleUniformScaler(.6f));
    dragger_->setPart("rotator1.rotator", new SoBigTransformBoxRotatorRotator(.4f));
    dragger_->setPart("rotator2.rotator", new SoBigTransformBoxRotatorRotator(.4f));
    dragger_->setPart("rotator3.rotator", new SoBigTransformBoxRotatorRotator(.4f));

    drag_trans_calc_ = new SoCalculator;
    drag_trans_calc_->ref();
    drag_trans_calc_->A.connectFrom(&dragger_->translation);

    // expr set in render below
    //   String expr = "oA = vec3f(" + String(.5f * (width_ + 2.0f * frame_margin)) + " + " + String(drag_size)
    //     + " * A[0], .5 + " +  String(drag_size)
    //     + " * A[1], " + String(drag_size) + " * A[2])";

    //   drag_trans_calc_->expression = expr.chars();

    txfm_shape()->translation.connectFrom(&drag_trans_calc_->oA);
    txfm_shape()->rotation.connectFrom(&dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&dragger_->scaleFactor);

    dragger_->addFinishCallback(T3GraphViewNode_DragFinishCB, (void*)this);
  }

  SoSeparator* chld = childNodes();
  chld_mat_ = new SoMaterial;
  chld_mat_->diffuseColor.setValue(0, 0, 0); // black
  chld->addChild(chld_mat_);

  x_axis_sep_ = new SoSeparator;
  chld->addChild(x_axis_sep_);
  z_axis_sep_ = new SoSeparator;
  chld->addChild(z_axis_sep_);
  y_axes_sep_ = new SoSeparator;
  chld->addChild(y_axes_sep_);
  graphs_sep_ = new SoSeparator;
  chld->addChild(graphs_sep_);

  SoMaterial* mat = material(); //cache
  mat->diffuseColor.setValue(0.0f, 0.5f, 0.5f); // blue/green
  mat->transparency.setValue(0.5f);

  SoSeparator* ss = shapeSeparator(); // cache
  frame_ = new SoFrame(SoFrame::Ver);
  insertChildAfter(ss, frame_, material());
}

T3GraphViewNode::~T3GraphViewNode()
{
  chld_mat_ = NULL;
  x_axis_sep_ = NULL;
  z_axis_sep_ = NULL;
  y_axes_sep_ = NULL;
  graphs_sep_ = NULL;
}

void T3GraphViewNode::render() {
  float frmg2 = 2.0f * frame_margin;

  if(show_drag_) {
    String expr = "oA = vec3f(" + String(.5f * width_) + " + " + String(drag_size)
      + " * A[0], " + String(.5f) + " + " +  String(drag_size)
      + " * A[1], " + String(drag_size) + " * A[2])";

    drag_trans_calc_->expression = expr.chars();
  }

  frame_->setDimensions(width_ + frmg2, 1.0f + frmg2, frame_width, frame_width);
  txfm_shape()->translation.setValue(.5f * width_, .5f, 0.0f);
  // note: also change in DragFinishCB in qtso
  SoFont* font = captionFont(true);
  transformCaption(iVec3f(0.1f, -((float)font->size.getValue() + frame_margin), 0.0f)); // move caption below the frame
}

void T3GraphViewNode::setWidth(float wdth) {
  width_ = wdth;
  render();
}

