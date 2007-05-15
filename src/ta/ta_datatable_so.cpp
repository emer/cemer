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
#include <Inventor/nodes/SoComplexity.h>
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

#define PI 3.14159265

//////////////////////////
//   T3GridViewNode	//
//////////////////////////

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
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);
    drag_->xf_->translation.setValue(-frame_margin, 0.0f, 0.0f);

    txfm_shape()->translation.connectFrom(&(drag_->trans_calc_->oA));
    txfm_shape()->rotation.connectFrom(&(drag_->dragger_->rotation));
    txfm_shape()->scaleFactor.connectFrom(&(drag_->dragger_->scaleFactor));

    drag_->dragger_->addFinishCallback(T3GridViewNode_DragFinishCB, (void*)this);
  }
  else {
    drag_ = NULL;
  }

  stage_ = new SoSeparator;
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
    insertChildAfter(topSeparator(), drag_, transform());
  }
  
  SoMaterial* mat = material(); //cache
  mat->diffuseColor.setValue(frame_clr_r, frame_clr_g, frame_clr_b); // blue/green
  mat->transparency.setValue(frame_clr_tr);

  SoSeparator* ss = shapeSeparator(); // cache
  frame_ = new SoFrame(SoFrame::Ver);
  insertChildAfter(ss, frame_, material());

  float vert_off = .5f + frame_margin + 2.0f * frame_width;

  col_sb_tx_ = new SoTransform;
  col_sb_tx_->translation.setValue(0.0f, -vert_off, -frame_width); // Z is to go below label
  col_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
  topSeparator()->addChild(col_sb_tx_);
  col_scroll_bar_ = new SoScrollBar;
  topSeparator()->addChild(col_scroll_bar_);

  SoTransform* scrtx = new SoTransform;
  scrtx->translation.setValue(.5f * width_ + frame_margin + 2.0f * frame_width,
			      vert_off, frame_width);
  scrtx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), -.5f * PI); // vertical!
  topSeparator()->addChild(scrtx);
  row_scroll_bar_ = new SoScrollBar;
  topSeparator()->addChild(row_scroll_bar_);
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
    String expr = "oA = vec3f(" + String(.5f * width_) + " + A[0], "
      + String(.5f * (1.0f + frmg2)) + " + A[1], A[2])";

    drag_->trans_calc_->expression = expr.chars();
  }

  txlt_stage_->translation.setValue(0.0f, 1.0f + frame_margin, 0.0f);
  frame_->setDimensions(width_ + frmg2, 1.0f + frmg2, frame_width, frame_width);
  txfm_shape()->translation.setValue(.5f * width_, .5f * (1.0f + frmg2), 0.0f);
  col_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
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
  // doesn't seem to make much diff:
  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  labels->addChild(cplx);
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

  errbars = new SoLineSet();
  errbars->vertexProperty.setValue(new SoVertexProperty());
  line_sep->addChild(errbars);

  markerSet_ = NULL;
  marker_size_ = MEDIUM;
  assertMarkerSet();		// just be done with it..
//   initValueColorMode();
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
  // doesn't seem to make much diff:
  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  textSep_->addChild(cplx);
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

//////////////////////////
//   T3GraphViewNode	//
//////////////////////////

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
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);
    drag_->xf_->translation.setValue(-frame_margin, -frame_margin, 0.0f);

    txfm_shape()->translation.connectFrom(&drag_->trans_calc_->oA);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3GraphViewNode_DragFinishCB, (void*)this);

    topSeparator()->addChild(drag_);
  }
  else {
    drag_ = NULL;
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
  legend_sep_ = new SoSeparator;
  chld->addChild(legend_sep_);
  graphs_sep_ = new SoSeparator;
  chld->addChild(graphs_sep_);

  if(show_drag_) {		  // don't show frame either!
    SoMaterial* mat = material(); //cache
    mat->diffuseColor.setValue(frame_clr_r, frame_clr_g, frame_clr_b); // blue/green
    mat->transparency.setValue(frame_clr_tr);

    SoSeparator* ss = shapeSeparator(); // cache
    frame_ = new SoFrame(SoFrame::Ver);
    insertChildAfter(ss, frame_, material());
  }

  float vert_off = frame_margin + 2.0f * frame_width;

  row_sb_tx_ = new SoTransform;
  row_sb_tx_->translation.setValue(0.5f * width_, -vert_off, -frame_width); // Z is to go below label
  row_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
  topSeparator()->addChild(row_sb_tx_);
  row_scroll_bar_ = new SoScrollBar;
  topSeparator()->addChild(row_scroll_bar_);
}

T3GraphViewNode::~T3GraphViewNode()
{
  chld_mat_ = NULL;
  x_axis_sep_ = NULL;
  z_axis_sep_ = NULL;
  y_axes_sep_ = NULL;
  legend_sep_ = NULL;
  graphs_sep_ = NULL;
}

void T3GraphViewNode::render() {
  float frmg2 = 2.0f * frame_margin;

  if(show_drag_) {
    String expr = "oA = vec3f(" + String(.5f * width_) + " + A[0], .5f + A[1], A[2])";

    drag_->trans_calc_->expression = expr.chars();

    frame_->setDimensions(width_ + frmg2, 1.0f + frmg2, frame_width, frame_width);
    txfm_shape()->translation.setValue(.5f * width_, .5f, 0.0f);
  }
  // note: also change in DragFinishCB in qtso
  SoFont* font = captionFont(true);
  transformCaption(iVec3f(0.1f, -((float)font->size.getValue() + frame_margin), 0.0f)); // move caption below the frame
  float vert_off = frame_margin + 2.0f * frame_width;
  row_sb_tx_->translation.setValue(0.5f * width_, -vert_off, -frame_width); // Z is to go below label
  row_sb_tx_->scaleFactor.setValue(width_, 1.0f, 1.0f);
}

void T3GraphViewNode::setWidth(float wdth) {
  width_ = wdth;
  render();
}

