/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#include "datagraph_so.h"

#include <Inventor/SbLinear.h>
#include <Inventor/SbName.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoUnits.h>
#include <Inventor/nodes/SoVertexProperty.h>


#define PI 3.14159265

//////////////////////////////////
//	  T3Axis		//
//////////////////////////////////


SO_NODE_SOURCE(T3Axis);

void T3Axis::initClass()
{
  SO_NODE_INIT_CLASS(T3Axis, T3NodeLeaf, "T3NodeLeaf");
}

T3Axis::T3Axis(Axis ax, void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3Axis);
//  axis_length = 1.0f; // dummy/default
  axis_ = ax;

  SoSeparator* ss = this->shapeSeparator(); //cache

  lines = new SoLineSet();
  lines->vertexProperty.setValue(new SoVertexProperty());
  ss->addChild(lines);

  labels = new SoSeparator();
  ss->addChild(labels);

  labelFont_ = new SoFont();
  labelFont_->ref(); // we re-add to the labels group every clear

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
  case Y: just = SoAsciiText::RIGHT;  break;
  case Z: just = SoAsciiText::RIGHT;  break; //TODO: verify
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

T3GraphLine::T3GraphLine(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphLine);

  textSep_ = NULL;
  textColor_ = NULL;
  labelFont_ = new SoFont();
  labelFont_->ref(); // we re-add to the text group after clear
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
  initValueColorMode();

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
      mb.setValue(SoVertexProperty::PER_PART); // i.e., per line segment
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
      mb.setValue(SoVertexProperty::PER_VERTEX); // i.e., per point
      orderedRGBA.setNum(0); // must supply colors explicitly
    } else {
      mb.setValue(SoVertexProperty::OVERALL);
      // set one and only color
      orderedRGBA.setNum(1);
      orderedRGBA.set1Value(0, defColor_);
    }

  }
}

void T3GraphLine::lineTo(const iVec3f& to) {
  SoMFInt32& nv = lines->numVertices; // cache
  int idx = nv.getNum() - 1; // cache
  //must have a line going...
  if (idx < 0) {
    //TODO: output error msg
    return;
  }
  nv.startEditing();
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  point.startEditing();

  // add the new line vertex
  point.set1Value(point.getNum(), to.x, to.y, -to.z);

  // now, update # pts in list
  nv.set1Value(idx, nv[idx] + 1);

  point.finishEditing();
  nv.finishEditing();

}

void T3GraphLine::lineTo(const iVec3f& to, const T3Color& c) {
  // if color not the same as last time, need to do a moveto to start new line
  // and add new color to binding list
  uint32_t new_col = T3Color::makePackedRGBA(c.r, c.g, c.b);
  SoMFUInt32& orderedRGBA = ((SoVertexProperty*)lines->vertexProperty.getValue())->orderedRGBA;
  if (orderedRGBA.getNum() == 0) {
    //TODO: this is an error condition because drawing must start w/ a moveto
    return;
  } else {
    uint32_t prv_col = orderedRGBA[orderedRGBA.getNum() - 1];
    if (new_col != prv_col)
      moveTo(to, c);
  }

  SoMFInt32& nv = lines->numVertices; // cache
  int idx = nv.getNum() - 1; // cache
  //must have a line going...
  if (idx < 0) {
    //TODO: output error msg
    return;
  }
  nv.startEditing();
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;
  point.startEditing();

  // add the new line vertex
  point.set1Value(point.getNum(), to.x, to.y, -to.z);

  // now, update # pts in list
  nv.set1Value(idx, nv[idx] + 1);

  point.finishEditing();
  nv.finishEditing();

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
  SoMFUInt32& orderedRGBA = ((SoVertexProperty*)lines->vertexProperty.getValue())->orderedRGBA;
  orderedRGBA.set1Value(orderedRGBA.getNum(), new_col);

  markerAt(pt, style);
}

void T3GraphLine::moveTo(const iVec3f& pt) {
  SoMFInt32& nv = lines->numVertices; // cache
  SoMFVec3f& point = ((SoVertexProperty*)lines->vertexProperty.getValue())->vertex;

  nv.startEditing();
  point.startEditing();

  // add the new line vertex
  point.set1Value(point.getNum(), pt.x, pt.y, -pt.z);

  // now, add a new slot update # pts in list
  nv.set1Value(nv.getNum(), 1);

  point.finishEditing();
  nv.finishEditing();
}

void T3GraphLine::moveTo(const iVec3f& pt, const T3Color& c) {
  uint32_t new_col = T3Color::makePackedRGBA(c.r, c.g, c.b);
  // always add the new color, then add the point
  SoMFUInt32& orderedRGBA = ((SoVertexProperty*)lines->vertexProperty.getValue())->orderedRGBA;
  orderedRGBA.set1Value(orderedRGBA.getNum(), new_col);

  moveTo(pt);
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
//   T3Graph	//
//////////////////////////


SO_NODE_SOURCE(T3Graph);

void T3Graph::initClass()
{
  SO_NODE_INIT_CLASS(T3Graph, T3NodeParent, "T3NodeParent");
}

T3Graph::T3Graph(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3Graph);

  // set size/pos of cube -- move down to -1 y
//  txfm_shape()->translation.setValue(x/2.0f, h/2.0f - 1.0f, -y/2.0f);
}

T3Graph::~T3Graph()
{
}

/*todo void T3Graph::setDefaultCaptionTransform() {
  SbVec3f tran(0.0f, -1.0f, 0.0f);
  transformCaption(tran);
} */


