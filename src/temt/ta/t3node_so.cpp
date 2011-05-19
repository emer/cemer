// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "t3node_so.h"
#include "t3viewer.h"
#include "ta_geometry.h"

#include <QImage>

#include <Inventor/SbLinear.h>
#include <Inventor/SbName.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoUnits.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>

#define PI 3.14159265


//////////////////////////
//   T3Color		//
//////////////////////////

uint32_t T3Color::makePackedRGBA(float r, float g, float b, float a) {
  return makePackedRGBA((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
}

uint32_t T3Color::makePackedRGBA(int r, int g, int b, int a) {
  return ((r & 0xff) << 24) | ((g & 0xff) << 16) | ((b & 0xff) << 8) | (a & 0xff);
}

//////////////////////////
//   T3Node		//
//////////////////////////

SO_NODE_ABSTRACT_SOURCE(T3Node);

void T3Node::insertChildBefore(SoGroup* group, SoNode* child, SoNode* before) {
  if (before) {
    int newindex = group->findChild(before);
    if (newindex < 0) {
      SoDebugError::post("T3Node::insertChildBefore",
                    "'before' node not found: %s",
                    before->getName().getString());
      return;
    }
    group->insertChild(child, newindex);
  } else
    group->addChild(child);
}

void T3Node::insertChildAfter(SoGroup* group, SoNode* child, SoNode* after) {
  if (after) {
    int newindex = group->findChild(after) + 1;
    if (newindex <= 0) {
      SoDebugError::post("T3Node::insertChildAfter",
                    "'after' node not found: %s",
                    after->getName().getString());
      return;
    }
    group->insertChild(child, newindex); //note: index allowed to be at end (==count)
  } else
    group->insertChild(child, 0);
}

SoNode* T3Node::getNodeByName(SoGroup* group, const char* name) {
  SoNode* rval;
  for (int i = 0; i < group->getNumChildren(); ++i) {
    rval = group->getChild(i);
    if (rval->getName() == name)
      return rval;
  }
  return NULL;
}


void T3Node::initClass()
{
  SO_NODE_INIT_ABSTRACT_CLASS(T3Node, SoSeparator, "Separator");
}

T3Node::T3Node(T3DataView* dataView__)
  : dataView_(dataView__)
{
  SO_NODE_CONSTRUCTOR(T3Node);

//  this->setName("topSeparator");

  transform_ = new SoTransform;
  transform_->setName("transform");
  topSeparator()->addChild(transform_);

  txfm_shape_ = NULL; // created in subclass's constructor
  material_ = NULL; // ditto
  captionNode_ = NULL;
}

T3Node::~T3Node()
{
  transform_ = NULL;
  txfm_shape_ = NULL;
  material_ = NULL;
}

void T3Node::initCommon() { // shared code, can only be called in a subclass constructor
  txfm_shape_ = new SoTransform;
  txfm_shape_->setName("txfm_shape");
  shapeSeparator()->addChild(txfm_shape_);

  material_ = new SoMaterial;
  material_->setName("material");
  shapeSeparator()->addChild(material_);
}

void T3Node::addRemoveChildNode(SoNode* node, bool adding) {
  SoDebugError::post("T3Node[subclass]::addRemoveChildNode",
                "operation not supported for this class");
}

const char* T3Node::caption() {
  SoAsciiText* cn = captionNode(false);
  if (cn)
    return cn->string[0].getString();
  else
    return NULL;
}

SoAsciiText* T3Node::captionNode(bool auto_create) {
  if (captionNode_ || !auto_create) return captionNode_;
  SoSeparator* cs = captionSeparator(auto_create); //note: ac is true
  SoBaseColor* bc = new SoBaseColor;

  if (T3DataView* dview = dataView()) {
    T3DataViewFrame* fr = dview->GetFrame();
    iColor txtcolr = fr->GetTextColor();
    bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
  }
  else {
    bc->rgb.setValue(0, 0, 0); //black is default for text
  }
  insertChildBefore(cs, bc, NULL); // at end
  captionNode_ = new SoAsciiText();
//  captionNode_->setName("captionNode");
  insertChildBefore(cs, captionNode_, NULL); // at end
  setDefaultCaptionTransform();
  return captionNode_;
}

SoTransform* T3Node::captionTransform(bool auto_create) {
  SoSeparator* cs = captionSeparator(auto_create);
  if (!cs) return NULL;
  SoTransform* rval = (SoTransform*)getNodeByName(cs, "captionTransform");;
  if (!rval && auto_create) {
    rval = new SoTransform();
    rval->setName("captionTransform");
    cs->insertChild(rval, 0); // first after sep
  }
  return rval;
}

void T3Node::setDefaultCaptionTransform() {
  transformCaption(iVec3f(0.0f, -0.02f, 0.02f));
  resizeCaption(.05f);	// default size
}

void T3Node::setCaption(const char* value) {
  int len = (value) ? (int)strlen(value) : 0; //64: can't possibly be >4G
  // only force getting captionNode if a non-blank value
  SoAsciiText* cn = captionNode(len > 0);
  // note: if node exists, we don't check for same value, so it will trigger a redraw
  // even if it is the same -- this could be changed if desired
  if (cn) {
    cn->string.setValue(value);
    setDefaultCaptionTransform();
  }
}

void T3Node::resizeCaption(float sz) {
  SoFont* font = captionFont(true);
  font->size.setValue(sz);
}

void T3Node::transformCaption(const iVec3f& translate)
{
  transformCaption(NULL, &translate);
}

void T3Node::transformCaption(const SbRotation& rotate, const iVec3f& translate)
{
  transformCaption(&rotate, &translate);
}

void T3Node::transformCaption(const SbRotation* rotate, const iVec3f* translate) {
  SoTransform* txfm = captionTransform(true);
  if (rotate) {
    txfm->rotation.setValue(*rotate);
  }
  if (translate)
    txfm->translation.setValue(SbVec3f(translate->x, translate->y, translate->z));
}


//////////////////////////
//   T3NodeLeaf		//
//////////////////////////

SO_NODE_SOURCE(T3NodeLeaf);

void T3NodeLeaf::initClass()
{
  SO_NODE_INIT_CLASS(T3NodeLeaf, T3Node, "");
}

T3NodeLeaf::T3NodeLeaf(T3DataView* dataView_)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3NodeLeaf);

  initCommon();
}

T3NodeLeaf::~T3NodeLeaf()
{
}

SoFont* T3NodeLeaf::captionFont(bool auto_create) {
  SoSeparator* cs = captionSeparator(auto_create);
  if (!cs) return NULL;
  SoFont* rval = (SoFont*)getNodeByName(cs, "captionFont");;
  if (!rval && auto_create) {
    rval = new SoFont();
    rval->setName("captionFont");
    rval->name = (const char*)taMisc::t3d_font_name;
    cs->insertChild(rval, 0);
  }
  return rval;
}

SoSeparator* T3NodeLeaf::captionSeparator(bool auto_create) {
  SoSeparator* ts = topSeparator();
  SoSeparator* rval = (SoSeparator*)getNodeByName(ts, "captionSeparator");
  if (!rval && auto_create) {
    rval = new SoSeparator;
    rval->setName("captionSeparator");
    insertChildAfter(ts, rval, transform());
  }
  return rval;
}


//////////////////////////
//   T3NodeParent	//
//////////////////////////

SO_NODE_SOURCE(T3NodeParent);

void T3NodeParent::initClass()
{
  SO_NODE_INIT_CLASS(T3NodeParent, T3Node, "");
}

T3NodeParent::T3NodeParent(T3DataView* dataView_)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3NodeParent);

  childNodes_ = NULL;

  shapeSeparator_ = new SoSeparator;
  shapeSeparator_->setName("shapeSeparator");
  topSeparator()->addChild(shapeSeparator_);

  initCommon();

}

T3NodeParent::~T3NodeParent()
{
  shapeSeparator_ = NULL;
  childNodes_ = NULL;
}

void T3NodeParent::addRemoveChildNode(SoNode* node, bool adding) {
  if (adding)
    childNodes()->addChild(node);
  else
    childNodes()->removeChild(node);
}

SoSeparator* T3NodeParent::captionSeparator(bool auto_create) {
  SoSeparator* ts = topSeparator();
  SoSeparator* rval = (SoSeparator*)getNodeByName(ts, "captionSeparator");
  if (!rval && auto_create) {
    rval = new SoSeparator;
    rval->setName("captionSeparator");
    // node goes after font, if any, else after shape s
    SoNode* after = getNodeByName(ts, "captionFont");
    if (!after)
      after = shapeSeparator();
    insertChildAfter(ts, rval, after);
  }
  return rval;
}

SoSeparator* T3NodeParent::childNodes() {
  if (!childNodes_) {
    childNodes_ = new SoSeparator;
    childNodes_->setName("childNodes");
    topSeparator()->addChild(childNodes_);
  }
  return childNodes_;
}

SoFont* T3NodeParent::captionFont(bool auto_create) {
  SoSeparator* ts = topSeparator();
  SoFont* rval = (SoFont*)getNodeByName(ts, "captionFont");;
  if (!rval && auto_create) {
    rval = new SoFont();
    rval->setName("captionFont");
    rval->name = (const char*)taMisc::t3d_font_name;
    insertChildAfter(ts, rval, shapeSeparator());
  }
  return rval;
}


//////////////////////////
//   SoFrame		//
//////////////////////////

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



//////////////////////////
//   SoRect		//
//////////////////////////

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

//////////////////////////
//   SoLineBox3d	//
//////////////////////////

SO_NODE_SOURCE(SoLineBox3d);

void SoLineBox3d::initClass()
{
  SO_NODE_INIT_CLASS(SoLineBox3d, SoIndexedLineSet, "SoIndexedLineSet");
}

SoLineBox3d::SoLineBox3d(float wd, float ht, float dp, bool ctr) {
  SO_NODE_CONSTRUCTOR(SoLineBox3d);

  center = ctr;
  width = wd;
  height = ht;
  depth = dp;

  vertexProperty.setValue(new SoVertexProperty); // note: vp refs/unrefs automatically
  render();
}

void SoLineBox3d::render() {
  SoMFVec3f& vtx = ((SoVertexProperty*)vertexProperty.getValue())->vertex;
  vtx.setNum(8);
  SbVec3f* v_dat = vtx.startEditing();
  int idx = 0;
  if(center) {
    float hx = 0.5f * width; float hy = 0.5f * height; float hz = 0.5f * depth;
    v_dat[idx++].setValue(-hx,-hy,-hz);	// around the back
    v_dat[idx++].setValue(hx,-hy,-hz);
    v_dat[idx++].setValue(hx,hy,-hz);
    v_dat[idx++].setValue(-hx,hy,-hz);
    v_dat[idx++].setValue(-hx,-hy,hz);	// around the front
    v_dat[idx++].setValue(hx,-hy,hz);
    v_dat[idx++].setValue(hx,hy,hz);
    v_dat[idx++].setValue(-hx,hy,hz);
  }
  else {
    v_dat[idx++].setValue(0.0f,0.0f,-depth);	// around the back
    v_dat[idx++].setValue(width,0.0f,-depth);
    v_dat[idx++].setValue(width,height,-depth);
    v_dat[idx++].setValue(0.0f,height,-depth);
    v_dat[idx++].setValue(0.0f,0.0f,0.0f);	// around the front
    v_dat[idx++].setValue(width,0.0f,0.0f);
    v_dat[idx++].setValue(width,height,0.0f);
    v_dat[idx++].setValue(0.0f,height,0.0f);
  }
  vtx.finishEditing();

  coordIndex.setNum(24);
  int32_t* c_dat = coordIndex.startEditing();
  idx = 0;
  c_dat[idx++] = 0;		// around the back
  c_dat[idx++] = 1;
  c_dat[idx++] = 2;
  c_dat[idx++] = 3;
  c_dat[idx++] = 0;
  c_dat[idx++] = -1;		// 6

  c_dat[idx++] = 4;		// around the front
  c_dat[idx++] = 5;
  c_dat[idx++] = 6;
  c_dat[idx++] = 7;
  c_dat[idx++] = 4;
  c_dat[idx++] = -1;		// 6

  c_dat[idx++] = 0;		// fb poles
  c_dat[idx++] = 4;
  c_dat[idx++] = -1;		// 3
  c_dat[idx++] = 1;
  c_dat[idx++] = 5;
  c_dat[idx++] = -1;		// 3
  c_dat[idx++] = 2;
  c_dat[idx++] = 6;
  c_dat[idx++] = -1;		// 3
  c_dat[idx++] = 3;
  c_dat[idx++] = 7;
  c_dat[idx++] = -1;		// 3
  // total = 2*6 + 4*3 = 24
  coordIndex.finishEditing();
}

////////////////////////////////////////////////////////////////////////////////////////////
// special Coin 3.0 code to set the read image callback so we can nuke simage for good!!!

#include <Inventor/SbBasic.h>

#if COIN_MAJOR_VERSION >= 3
#include <Inventor/SbImage.h>

// this code is essentially verbatim from Quarter
// http://svn.coin3d.org/repos/Quarter/trunk/src/Quarter/ImageReader.cpp

CoinImageReaderCB::CoinImageReaderCB(void)
{
  SbImage::addReadImageCB(CoinImageReaderCB::readImageCB, this);
}

CoinImageReaderCB::~CoinImageReaderCB(void)
{
  SbImage::removeReadImageCB(CoinImageReaderCB::readImageCB, this);
}

SbBool
CoinImageReaderCB::readImage(const SbString & filename, SbImage & sbimage) const
{
  QImage image;
  if (image.load(filename.getString())) {
    int c;
    int w = image.width();
    int h = image.height();

    // Keep in 8-bits mode if that was what we read
    if (image.depth() != 8 || !image.isGrayscale()) {
      // FIXME: consider if we should detect allGrayscale() and alpha (c = 2)
      image = image.convertToFormat(image.hasAlphaChannel() ?
                                    QImage::Format_ARGB32 : QImage::Format_RGB32);
    }

    { // QtCoinCompatibility::QImageToSbImage(const QImage & image, SbImage & sbimage)
      // Keep in 8-bits mode if that was what we read
      if (image.depth() == 8 && image.isGrayscale()) {
	c = 1;
      }
      else {
	// FIXME: consider if we should detect allGrayscale() and alpha (c = 2)
	c = image.hasAlphaChannel() ? 4 : 3;
      }

      SbVec2s size((short) w, (short) h);
      sbimage.setValue(size, c, NULL);
      unsigned char * buffer = sbimage.getValue(size, c);

      if (c == 1) {
	for (int i = 0; i < h; i++) {
	  memcpy(buffer + i*w, image.scanLine(h-(i+1)), w);
	}
      }
      else { // (c == 3 || c == 4)
	int max_idx = c * w * (h-1);
	QRgb * bits = (QRgb*) image.bits();
	for (int y = 0; y < h; y++) {
	  int idx = c*w*(h-(y+1));
	  if(idx > max_idx || idx < 0) {
	    continue;
	  }
	  unsigned char * line = &buffer[idx];
	  for (int x = 0; x < w; x++) {
	    *line = qRed(*bits);  line++;
	    *line = qGreen(*bits); line++;
	    *line = qBlue(*bits); line++;
	    if (c == 4) {
	      *line = qAlpha(*bits); line++;
	    }
	    bits++;
	  }
	}
      }
    }
    return TRUE;
  }
  return FALSE;
}

SbBool
CoinImageReaderCB::readImageCB(const SbString & filename, SbImage * image, void * closure)
{
  return ((CoinImageReaderCB*)closure)->readImage(filename, *image);
}


static CoinImageReaderCB coin_img_reader_cb; // always have one statically -- does this work??

#else // COIN_MAJOR_VERSION >= 3

// no-ops

CoinImageReaderCB::CoinImageReaderCB(void)
{
}

CoinImageReaderCB::~CoinImageReaderCB(void)
{
}

SbBool
CoinImageReaderCB::readImage(const SbString & filename, SbImage & sbimage) const
{
  return FALSE;
}

SbBool
CoinImageReaderCB::readImageCB(const SbString & filename, SbImage * image, void * closure)
{
  return FALSE;
}

#endif // COIN_MAJOR_VERSION >= 3


//////////////////////////
//   SoImageEx		//
//////////////////////////

SO_NODE_SOURCE(SoImageEx);

// copy 32 bit xRGB data to an intermediate packed buffer
// then to the Texture
void xRGB_to_Texture(const QImage& img, SoTexture2* sotx) {
  const int nc = 3;
  int src_cnt = img.width() * img.height();
  unsigned char* buf = (unsigned char*)malloc(src_cnt * nc);
  int dst_i = 0;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = img.height() - 1; y >= 0; --y)
  for (int x = 0; x < img.width(); ++x) {
    QRgb rgb = img.pixel(x, y);
    buf[dst_i++] = (unsigned char)qRed(rgb);
    buf[dst_i++] = (unsigned char)qGreen(rgb);
    buf[dst_i++] = (unsigned char)qBlue(rgb);
  }
  sotx->image.setValue(SbVec2s(img.width(), img.height()),
    nc, buf, SoSFImage::COPY);
  free(buf);
}

void ARGB_to_Texture(const QImage& img, SoTexture2* sotx) {
  const int nc = 4;
  int src_cnt = img.width() * img.height();
  unsigned char* buf = (unsigned char*)malloc(src_cnt * nc);
  int dst_i = 0;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = img.height() - 1; y >= 0; --y)
  for (int x = 0; x < img.width(); ++x) {
    QRgb rgb = img.pixel(x, y);
    buf[dst_i++] = (unsigned char)qRed(rgb);
    buf[dst_i++] = (unsigned char)qGreen(rgb);
    buf[dst_i++] = (unsigned char)qBlue(rgb);
    buf[dst_i++] = (unsigned char)qAlpha(rgb);
  }
  sotx->image.setValue(SbVec2s(img.width(), img.height()),
    nc, buf, SoSFImage::COPY);
  free(buf);
}

bool SoImageEx::SetTextureImage(SoTexture2* sotx, const QImage& qimg) {
  // TODO: would be *great* to search paths for fname
  // assume it is a RGB or ARGB -- if latter, we'll need to rotate to Coin's RGBA
  QImage::Format format = qimg.format();
  // note: loading should never return the normalized alpha format...
  if (format == QImage::Format_ARGB32) {
    ARGB_to_Texture(qimg, sotx);
    return true; 
  } 
  QImage qimg2(qimg); // may not need to be changed
  if (format != QImage::Format_RGB32) {
    qimg2 = qimg.convertToFormat(QImage::Format_RGB32);
  }
  xRGB_to_Texture(qimg2, sotx);
  return true;
}

bool SoImageEx::SetTextureFile(SoTexture2* sotx, const String& fname) {
  // don't use simage under any condition
  return SetTextureFile_impl(sotx, fname, false);
}

bool SoImageEx::SetTextureFile_impl(SoTexture2* sotx, const String& fname,
  bool use_simage) 
{
  if (use_simage) {		// generally not used
    sotx->filename = (const char*)fname;
    // check if anything loaded
    SbVec2s size; 
    int nc = 0;
    sotx->image.getValue(size, nc);
    return (nc != 0); // would be zero on failure
  } 
  // else Qt 
  QImage qqimg;
  if (!qqimg.load(fname)) return false;
  return SetTextureImage(sotx, qqimg);
}

void SoImageEx::initClass()
{
  SO_NODE_INIT_CLASS(SoImageEx, SoSeparator, "SoSeparator");
}

SoImageEx::SoImageEx() {
  SO_NODE_CONSTRUCTOR(SoImageEx);
  texture = new SoTexture2;
  this->addChild(texture);
  // we set shape to white, so texture map modulates it properly
  SoBaseColor* bc = new SoBaseColor;
  bc->rgb.setValue(0xFF, 0xFF, 0xFF);
  this->addChild(bc);
  shape = new SoRect; //note: w,h = 1.0 so texture map maps exactly
  this->addChild(shape);
}

SoImageEx::~SoImageEx() {
  texture = NULL;
  shape = NULL;
}

void SoImageEx::adjustScale() {
  // just always keep it a square with the x a tiny bit larger than the y -- this seems to
  // prevent spurious rotations and keeps it displayed the same way every time.  geez.
  shape->setDimensions(1.0f, 0.9999f);
  // set proper type
  if (img.dims() == 2) {
    texture->model = SoTexture2::REPLACE;
  } else {
    texture->model = SoTexture2::DECAL;
  }
}

void SoImageEx::setImage(const QImage& src) {
  if (src.isGrayscale())
    setImage2(src);
  else 
    setImage3(src);
  adjustScale();
}

void SoImageEx::setImage2(const QImage& src) {
  d.x = src.width();
  d.y = src.height();
  img.SetGeom(2, d.x, d.y);
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = d.y - 1; y >= 0; --y) {
    for (int x = 0; x < d.x; ++x) {
      img.FastEl(x,y) = (byte)(qGray(src.pixel(x, y)));
    }
  }
  texture->image.setValue(SbVec2s(d.y, d.x), 1, (const unsigned char*)img.data(),
			  SoSFImage::NO_COPY);
}

void SoImageEx::setImage3(const QImage& src) {
  d.x = src.width();
  d.y = src.height();
  //NOTE: img geom is not same as input: rgb is in innermost for us
  img.SetGeom(3, 3, d.x, d.y);
  int idx = 0;
  QRgb rgb;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = d.y - 1; y >= 0; --y) {
    for (int x = 0; x < d.x; ++x) {
      rgb = src.pixel(x, y);
      img.FastEl_Flat(idx++) = (byte)(qRed(rgb));
      img.FastEl_Flat(idx++) = (byte)(qGreen(rgb));
      img.FastEl_Flat(idx++) = (byte)(qBlue(rgb));
    }
  }
  texture->image.setValue(SbVec2s(d.x, d.y), 3, (const unsigned char*)img.data(),
			  SoSFImage::NO_COPY);
}

void SoImageEx::setImage(const taMatrix& src, bool top_zero) {
  int dims = src.dims(); //cache
  if (dims == 2) {
    setImage2(src, top_zero);
  } else if (((dims == 3) && (src.dim(2) == 3))) { 
    setImage3(src, top_zero);
  } else { 
    taMisc::Error("SoImageEx::setImage: must be gray (dims=2) or rgb matrix");
    return;
  }
  adjustScale();
}

void SoImageEx::setImage2(const taMatrix& src, bool top_zero) {
  d.x = src.dim(0);
  d.y = src.dim(1);
//   if(src.GetDataValType() == taBase::VT_BYTE) {
//     texture->image.setValue(SbVec2s(d.x, d.y), 1, (const unsigned char*)src.data());
//   }
  if((src.GetDataValType() == taBase::VT_FLOAT) ||
	  (src.GetDataValType() == taBase::VT_DOUBLE)) {
    img.SetGeom(2, d.x, d.y);
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y) * 255.0f);
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x,y) = (byte)(src.FastElAsFloat(x, y) * 255.0f);
	}
      }
    }
    texture->image.setValue(SbVec2s(d.x, d.y), 1, (const unsigned char*)img.data(),
			    SoSFImage::NO_COPY);
  }
  else {			// int type -- assume raw value
    img.SetGeom(2, d.x, d.y);
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x, d.y - 1 - y) = (byte)src.FastElAsFloat(x, y);
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x,y) = (byte)src.FastElAsFloat(x, y);
	}
      }
    }
    texture->image.setValue(SbVec2s(d.x, d.y), 1, (const unsigned char*)img.data(),
			    SoSFImage::NO_COPY);
  }
}

void SoImageEx::setImage3(const taMatrix& src, bool top_zero) {
  d.x = src.dim(0);
  d.y = src.dim(1);
  //NOTE: img geom is not same as input: rgb is in innermost for us
  img.SetGeom(3, 3, d.x, d.y);
  if((src.GetDataValType() == taBase::VT_FLOAT) ||
     (src.GetDataValType() == taBase::VT_DOUBLE)) {
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 0) * 255.0f);
	  img.FastEl(1,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 1) * 255.0f);
	  img.FastEl(2,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 2) * 255.0f);
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,y) = (byte)(src.FastElAsFloat(x, y, 0) * 255.0f);
	  img.FastEl(1,x,y) = (byte)(src.FastElAsFloat(x, y, 1) * 255.0f);
	  img.FastEl(2,x,y) = (byte)(src.FastElAsFloat(x, y, 2) * 255.0f);
	}
      }
    }
  }
  else {			// int type -- assume raw value
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 0));
	  img.FastEl(1,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 1));
	  img.FastEl(2,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 2));
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,y) = (byte)(src.FastElAsFloat(x, y, 0));
	  img.FastEl(1,x,y) = (byte)(src.FastElAsFloat(x, y, 1));
	  img.FastEl(2,x,y) = (byte)(src.FastElAsFloat(x, y, 2));
	}
      }
    }
  }
  texture->image.setValue(SbVec2s(d.x, d.y), 3, (const unsigned char*)img.data(),
			  SoSFImage::NO_COPY);
}

//////////////////////////
//   SoMatrixGrid	//
//////////////////////////

SO_NODE_SOURCE(SoMatrixGrid);

void SoMatrixGrid::initClass()
{
  SO_NODE_INIT_CLASS(SoMatrixGrid, SoSeparator, "SoSeparator");
}

SoMatrixGrid::SoMatrixGrid(taMatrix* mat, bool oddy, ColorScale* sc, MatrixLayout layout, bool val_txt) {
  SO_NODE_CONSTRUCTOR(SoMatrixGrid);

  transform_ = new SoTransform;
  addChild(transform_);
  shape_ = new SoIndexedTriangleStripSet;
  addChild(shape_);
  vtx_prop_ = new SoVertexProperty;
  shape_->vertexProperty.setValue(vtx_prop_);
  cell_text_ = NULL;
  cell_fnt_ = NULL;

  matrix = mat;
  taBase::Ref(matrix);
  odd_y = oddy;
  scale = sc;
  mat_layout = layout;
  val_text = val_txt;

  max_txt_len = 6;
  spacing = .1f;
  block_height = .2f;
  trans_max = 0.6f;
  user_data = NULL;
  //  render(); // don't do this by default; often want to spec further guys
}

SoMatrixGrid::~SoMatrixGrid() {
  taBase::UnRef(matrix);
}

void SoMatrixGrid::setMatrix(taMatrix* mat, bool oddy) { 
  taBase::UnRef(matrix);
  matrix = mat; 
  taBase::Ref(matrix);
  odd_y = oddy;
  render();
}

void SoMatrixGrid::setColorScale(ColorScale* cs) { 
  scale = cs;
  render();
}

void SoMatrixGrid::setLayout(MatrixLayout layout) { 
  mat_layout = layout;
  render();
}

void SoMatrixGrid::setValText(bool val_txt) { 
  val_text = val_txt;
  render();
}

void SoMatrixGrid::setBlockHeight(float bh) { 
  block_height = bh;
  render();
}

void SoMatrixGrid::ValToDispText(float val, String& str) {
  float val_abs = fabsf(val);
  if ((val_abs < .0001) || (val_abs >= 10000))
    str.convert(val, "%7.1e"); //note: 7 chars
  if (val_abs < 1)
    str.convert(val, "%#6.4f");
  else if (val_abs < 10)
    str.convert(val, "%#6.3f");
  else if (val_abs < 100)
    str.convert(val, "%#6.2f");
  else if (val_abs < 1000)
    str.convert(val, "%#6.1f");
  else //must be: if (val_abs < 10000)
    str.convert(val, "6.0f");
}

void SoMatrixGrid::render() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFVec3f& normal = vtx_prop_->normal;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;

  vtx_prop_->normalBinding.setValue(SoNormalBinding::PER_FACE_INDEXED);
  vtx_prop_->materialBinding.setValue(SoMaterialBinding::PER_PART_INDEXED);

  normal.setNum(5);
  SbVec3f* normal_dat = normal.startEditing();
  int idx=0;
  normal_dat[idx++].setValue(0.0f,  -1.0f, 0.0f); // back = 0
  normal_dat[idx++].setValue(1.0f,  0.0f , 0.0f); // right = 1
  normal_dat[idx++].setValue(-1.0f, 0.0f , 0.0f); // left = 2
  normal_dat[idx++].setValue(0.0f,  1.0f , 0.0f); // front = 3
  normal_dat[idx++].setValue(0.0f,  0.0f , 1.0f); // top = 4
  normal.finishEditing();

  int geom_x, geom_y;
  matrix->geom.Get2DGeomGui(geom_x, geom_y, odd_y, 1);
//SB?  matrix->geom.Get2DGeom(geom_x, geom_y);
  float cl_x = 1.0f / (float)geom_x;	// how big each cell is
  float cl_y = 1.0f / (float)geom_y;
  max_xy = MAX(cl_x, cl_y);
  cl_spc = spacing * max_xy;
  blk_ht = block_height * max_xy;
  
  int n_geom = matrix->count();	// not always x * y due to spaces in display geom
  int n_per_vtx = 8;
  int tot_vtx =  n_geom * n_per_vtx;
  vertex.setNum(tot_vtx);
  color.setNum(n_geom);

  bool build_text = false;
  float ufontsz = MIN(1.5f * (cl_x / (float)max_txt_len), cl_y);
  if(!val_text) {
    if (cell_text_) {
      removeChild(cell_text_);
      cell_fnt_ = NULL;
      cell_text_ = NULL;
    }
  }
  else {
    if(!cell_text_) {
      cell_text_ = new SoSeparator;
      build_text = true;
      addChild(cell_text_);
      SoBaseColor* bc = new SoBaseColor;
      bc->rgb.setValue(0, 0, 0); //black is default for text
      cell_text_->addChild(bc);
      cell_fnt_ = new SoFont();
      cell_fnt_->name = (const char*)taMisc::t3d_font_name;
      cell_text_->addChild(cell_fnt_);
    }
    cell_fnt_->size.setValue(ufontsz);
  }

  SbVec3f* vertex_dat = vertex.startEditing();

  String val_str;
  T3Color col;
  TwoDCoord pos;
  int v_idx = 0;
  int t_idx = 2;		// base color + font
  // these go in normal order; indexes are backwards

  if(matrix->dims() <= 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = ((float)pos.x + cl_spc) * cl_x;
	float yp = ((float)pos.y + cl_spc) * cl_y;
	float xp1 = ((float)pos.x+1 - cl_spc) * cl_x;
	float yp1 = ((float)pos.y+1 - cl_spc) * cl_y;
	yp = 1.0f - yp; yp1 = 1.0f - yp1; // always flip y
	float zp = blk_ht;
	vertex_dat[v_idx++].setValue(xp,  yp , 0.0f); // 00_0 = 0
	vertex_dat[v_idx++].setValue(xp1, yp , 0.0f); // 10_0 = 0
	vertex_dat[v_idx++].setValue(xp,  yp1, 0.0f); // 01_0 = 0
	vertex_dat[v_idx++].setValue(xp1, yp1, 0.0f); // 11_0 = 0

	// zp will be updated later!
	vertex_dat[v_idx++].setValue(xp,  yp , zp); // 00_v = 1
	vertex_dat[v_idx++].setValue(xp1, yp , zp); // 10_v = 2
	vertex_dat[v_idx++].setValue(xp,  yp1, zp); // 01_v = 3
	vertex_dat[v_idx++].setValue(xp1, yp1, zp); // 11_v = 4

	if(val_text) {
	  render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	}
      }
    }
  }
  else if(matrix->dims() == 3) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int zmax = matrix->dim(2);
    for(int z=0; z<zmax; z++) {
      for(pos.y=0; pos.y<ymax; pos.y++) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  TwoDCoord apos = pos;
	  if(odd_y)
	    apos.y += z * (ymax+1);
	  else
	    apos.x += z * (xmax+1);
	  float xp = ((float)apos.x + cl_spc) * cl_x;
	  float yp = ((float)apos.y + cl_spc) * cl_y;
	  float xp1 = ((float)apos.x+1 - cl_spc) * cl_x;
	  float yp1 = ((float)apos.y+1 - cl_spc) * cl_y;
	  float zp = blk_ht;
	  yp = 1.0f - yp; yp1 = 1.0f - yp1;		// always flip y
	  vertex_dat[v_idx++].setValue(xp,  yp , 0.0f); // 00_0 = 0
	  vertex_dat[v_idx++].setValue(xp1, yp , 0.0f); // 10_0 = 0
	  vertex_dat[v_idx++].setValue(xp,  yp1, 0.0f); // 01_0 = 0
	  vertex_dat[v_idx++].setValue(xp1, yp1, 0.0f); // 11_0 = 0

	  // zp will be updated later!
	  vertex_dat[v_idx++].setValue(xp,  yp , zp); // 00_v = 1
	  vertex_dat[v_idx++].setValue(xp1, yp , zp); // 10_v = 2
	  vertex_dat[v_idx++].setValue(xp,  yp1, zp); // 01_v = 3
	  vertex_dat[v_idx++].setValue(xp1, yp1, zp); // 11_v = 4

	  if(val_text) {
	    render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	  }
	}
      }
    }
  }
  else if(matrix->dims() == 4) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int xxmax = matrix->dim(2);
    int yymax = matrix->dim(3);
    TwoDCoord opos;
    for(opos.y=0; opos.y<yymax; opos.y++) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=0; pos.y<ymax; pos.y++) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    TwoDCoord apos = pos;
	    apos.x += opos.x * (xmax+1);
	    apos.y += opos.y * (ymax+1);
	    float xp = ((float)apos.x + cl_spc) * cl_x;
	    float yp = ((float)apos.y + cl_spc) * cl_y;
	    float xp1 = ((float)apos.x+1 - cl_spc) * cl_x;
	    float yp1 = ((float)apos.y+1 - cl_spc) * cl_y;
	    float zp = blk_ht;
	    yp = 1.0f - yp; yp1 = 1.0f - yp1;		  // always flip 
	    vertex_dat[v_idx++].setValue(xp,  yp , 0.0f); // 00_0 = 0
	    vertex_dat[v_idx++].setValue(xp1, yp , 0.0f); // 10_0 = 0
	    vertex_dat[v_idx++].setValue(xp,  yp1, 0.0f); // 01_0 = 0
	    vertex_dat[v_idx++].setValue(xp1, yp1, 0.0f); // 11_0 = 0

	    // zp will be updated later!
	    vertex_dat[v_idx++].setValue(xp,  yp , zp); // 00_v = 1
	    vertex_dat[v_idx++].setValue(xp1, yp , zp); // 10_v = 2
	    vertex_dat[v_idx++].setValue(xp,  yp1, zp); // 01_v = 3
	    vertex_dat[v_idx++].setValue(xp1, yp1, zp); // 11_v = 4

	    if(val_text) {
	      render_text(build_text, t_idx, xp, xp1, yp, yp1, zp, ufontsz);
	    }
          }
	}
      }
    }
  }
  
  vertex.finishEditing();

  // todo: could cleanup cell_text_ child list if extras, but not clear if needed

  SoMFInt32& coords = shape_->coordIndex;
  SoMFInt32& norms = shape_->normalIndex;
  SoMFInt32& mats = shape_->materialIndex;
  int nc_per_idx = 19;		// number of coords per index
  int nn_per_idx = 10;		// number of norms per index
  int nm_per_idx = 3;		// number of mats per index
  coords.setNum(n_geom * nc_per_idx);
  norms.setNum(n_geom * nn_per_idx);
  mats.setNum(n_geom * nm_per_idx);

  // values of the cubes xy_[0,v]
  //     01_v   11_v   
  //   01_0   11_0     
  //     00_v   10_v    
  //   00_0   10_0     

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4

  int32_t* coords_dat = coords.startEditing();
  int32_t* norms_dat = norms.startEditing();
  int32_t* mats_dat = mats.startEditing();
  int cidx = 0;
  int nidx = 0;
  int midx = 0;
  if(matrix->dims() <= 2) {
    int nx = geom_x;
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	int mat_idx = (pos.y * nx + pos.x);
	int c00_0 = mat_idx * n_per_vtx;
	render_block_idx(c00_0, mat_idx, coords_dat, norms_dat, mats_dat, cidx, nidx, midx);
      }
    }
  }
  else if(matrix->dims() == 3) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int zmax = matrix->dim(2);
    for(int z=zmax-1; z>=0; z--) {
      for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  int mat_idx = matrix->FastElIndex(pos.x, pos.y, z);
	  int c00_0 = mat_idx * n_per_vtx;
	  render_block_idx(c00_0, mat_idx, coords_dat, norms_dat, mats_dat, cidx, nidx, midx);
        }
      }
    }
  }
  else if(matrix->dims() == 4) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int xxmax = matrix->dim(2);
    int yymax = matrix->dim(3);
    TwoDCoord opos;
    for(opos.y=yymax-1; opos.y>=0; opos.y--) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=ymax-1; pos.y>=0; pos.y--) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    int mat_idx = matrix->FastElIndex(pos.x, pos.y, opos.x, opos.y);
	    int c00_0 = mat_idx * n_per_vtx;
	    render_block_idx(c00_0, mat_idx, coords_dat, norms_dat, mats_dat, cidx, nidx, midx);
          }
        }
      }
    }
  }
  coords.finishEditing();
  norms.finishEditing();
  mats.finishEditing();

  renderValues();		// hand off to next guy..
}

void SoMatrixGrid::render_text(bool build_text, int& t_idx, float xp, float xp1,
			       float yp, float yp1, float zp, float ufontsz)
{
  if(build_text || cell_text_->getNumChildren() <= t_idx) {
    SoSeparator* tsep = new SoSeparator;
    cell_text_->addChild(tsep);
    SoTranslation* tr = new SoTranslation;
    tsep->addChild(tr);
    SoAsciiText* txt = new SoAsciiText();
    txt->justification = SoAsciiText::CENTER;
    tsep->addChild(txt);
  }
  SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
  SoTranslation* tr = (SoTranslation*)tsep->getChild(0);
  float xfp = .5f * (xp + xp1);
  float yfp = .5f * (yp + yp1) - .5f * ufontsz;
  tr->translation.setValue(xfp, yfp, MAX(zp,0.0f) + .01f);
  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
  txt->string.setValue("0.0");	// placeholder; gets set later
  t_idx++;
}

void SoMatrixGrid::render_block_idx(int c00_0, int mat_idx,
				    int32_t* coords_dat, int32_t* norms_dat,
				    int32_t* mats_dat, int& cidx, int& nidx, int& midx) {
  int c10_0 = c00_0 + 1; int c01_0 = c00_0 + 2; int c11_0 = c00_0 + 3;
  int c00_v = c00_0 + 4; int c10_v = c00_0 + 5; int c01_v = c00_0 + 6;
  int c11_v = c00_0 + 7;

  // back - right
  //     1    3
  //   0    2     
  //     x    5  
  //   x    4   

  coords_dat[cidx++] = (c01_0); // 0
  coords_dat[cidx++] = (c01_v); // 1
  coords_dat[cidx++] = (c11_0); // 2
  coords_dat[cidx++] = (c11_v); // 3
  coords_dat[cidx++] = (c10_0); // 4
  coords_dat[cidx++] = (c10_v); // 5
  coords_dat[cidx++] = (-1); // -1  -- 7 total

  norms_dat[nidx++] = (0); // back
  norms_dat[nidx++] = (0); // back
  norms_dat[nidx++] = (1); // right
  norms_dat[nidx++] = (1); // right -- 4 total

  mats_dat[midx++] = (mat_idx);

  // left - front
  //     1    x 
  //   0    x   
  //     3    5
  //   2    4   

  coords_dat[cidx++] = (c01_0); // 0
  coords_dat[cidx++] = (c01_v); // 1
  coords_dat[cidx++] = (c00_0); // 2
  coords_dat[cidx++] = (c00_v); // 3
  coords_dat[cidx++] = (c10_0); // 4
  coords_dat[cidx++] = (c10_v); // 5
  coords_dat[cidx++] = (-1); // -1 -- 7 total

  norms_dat[nidx++] = (2); // left
  norms_dat[nidx++] = (2); // left
  norms_dat[nidx++] = (3); // front
  norms_dat[nidx++] = (3); // front -- 4 total

  mats_dat[midx++] = (mat_idx);

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4
  // top
  //     0    1
  //   x    x  
  //     2    3
  //   x    x  

  coords_dat[cidx++] = (c01_v); // 0
  coords_dat[cidx++] = (c11_v); // 1
  coords_dat[cidx++] = (c00_v); // 2
  coords_dat[cidx++] = (c10_v); // 3
  coords_dat[cidx++] = (-1); // -1 -- 5 total

  norms_dat[nidx++] = (4); // top
  norms_dat[nidx++] = (4); // top -- 2 total

  mats_dat[midx++] = (mat_idx);

  // total coords = 7 + 7 + 5 = 19
  // total norms = 4 + 4 + 2 = 10
  // total mats = 3
}

void SoMatrixGrid::renderValues() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  int geom_x, geom_y;
  matrix->geom.Get2DGeomGui(geom_x, geom_y, odd_y, 1);
//SB?  matrix->geom.Get2DGeom(geom_x, geom_y);

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();

  String val_str;
  float val;
  float sc_val;
  T3Color col;
  TwoDCoord pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 2;		// base color + font

  // these go in normal order; indexes are backwards
  // note: only 2d case right yet..
  if(matrix->dims() == 1) {
    pos.x = 0;
    int ymax = matrix->dim(0);	// assumes odd_y
    for(pos.y=0; pos.y<ymax; pos.y++) {
      if(mat_layout == BOT_ZERO)
	val = matrix->FastElAsFloat(ymax-1-pos.y);
      else
	val = matrix->FastElAsFloat(pos.y);

      iColor fl;  iColor tx;
      scale->GetColor(val,sc_val,&fl,&tx);
      float zp = sc_val * blk_ht;
      v_idx+=4;			// skip the _0 cases
      for(int i=0;i<4;i++)
	vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
      float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
      color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
      if(val_text) {
	SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	if(matrix->GetDataValType() == taBase::VT_STRING) {
	  if(mat_layout == BOT_ZERO)
	    val_str = ((String_Matrix*)matrix)->FastEl(ymax-1-pos.y).elidedTo(max_txt_len);
	  else
	    val_str = ((String_Matrix*)matrix)->FastEl(pos.y).elidedTo(max_txt_len);
	}
	else {
	  ValToDispText(val, val_str);
	}
	txt->string.setValue(val_str.chars());
	t_idx++;
      }
    }
  }
  else if(matrix->dims() == 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	if(mat_layout == BOT_ZERO)
	  val = matrix->FastElAsFloat(pos.x, geom_y-1-pos.y);
	else
	  val = matrix->FastElAsFloat(pos.x, pos.y);
	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	float zp = sc_val * blk_ht;
	v_idx+=4;			// skip the _0 cases
	for(int i=0;i<4;i++)
	  vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
	float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
	color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	if(val_text) {
	  SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	  if(matrix->GetDataValType() == taBase::VT_STRING) {
	    if(mat_layout == BOT_ZERO)
	      val_str = ((String_Matrix*)matrix)->FastEl(pos.x, geom_y-1-pos.y).elidedTo(max_txt_len);
	    else
	      val_str = ((String_Matrix*)matrix)->FastEl(pos.x, pos.y).elidedTo(max_txt_len);
	  }
	  else {
	    ValToDispText(val, val_str);
	  }
	  txt->string.setValue(val_str.chars());
	  t_idx++;
	}
      }
    }
  }
  else if(matrix->dims() == 3) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int zmax = matrix->dim(2);
    for(int z=0; z<zmax; z++) {
      for(pos.y=0; pos.y<ymax; pos.y++) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  if(mat_layout == BOT_ZERO)
	    val = matrix->FastElAsFloat(pos.x, ymax-1-pos.y, zmax-1-z);
	  else
	    val = matrix->FastElAsFloat(pos.x, pos.y, z);
	  iColor fl;  iColor tx;
	  scale->GetColor(val,sc_val,&fl,&tx);
	  float zp = sc_val * blk_ht;
	  v_idx+=4;			// skip the _0 cases
	  for(int i=0;i<4;i++)
	    vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
	  float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
	  color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	  if(val_text) {
	    SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	    SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	    if(matrix->GetDataValType() == taBase::VT_STRING) { // todo: replicate if compiles
	      if(mat_layout == BOT_ZERO)
		val_str = ((String_Matrix*)matrix)->FastEl(pos.x, ymax-1-pos.y, zmax-1-z).elidedTo(max_txt_len);
	      else
		val_str = ((String_Matrix*)matrix)->FastEl(pos.x, pos.y, z).elidedTo(max_txt_len);
	    }
	    else {
	      ValToDispText(val, val_str);
	    }
	    txt->string.setValue(val_str.chars());
	    t_idx++;
	  }
	}
      }
    }
  }
  else if(matrix->dims() == 4) {
    int xmax = matrix->dim(0);
    int ymax = matrix->dim(1);
    int xxmax = matrix->dim(2);
    int yymax = matrix->dim(3);
    TwoDCoord opos;
    for(opos.y=0; opos.y<yymax; opos.y++) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=0; pos.y<ymax; pos.y++) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    if(mat_layout == BOT_ZERO)
	      val = matrix->FastElAsFloat(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
	    else
	      val = matrix->FastElAsFloat(pos.x, pos.y, opos.x, opos.y);
	    iColor fl;  iColor tx;
	    scale->GetColor(val,sc_val,&fl,&tx);
	    float zp = sc_val * blk_ht;
	    v_idx+=4;			// skip the _0 cases
	    for(int i=0;i<4;i++)
	      vertex_dat[v_idx++][2] = zp; // 00_v = 1, 10_v = 2, 01_v = 3, 11_v = 4
	    float alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans_max);
	    color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	    if(val_text) {
	      SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	      SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	      if(matrix->GetDataValType() == taBase::VT_STRING) {
		if(mat_layout == BOT_ZERO)
		  val_str = ((String_Matrix*)matrix)->FastEl(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y).elidedTo(max_txt_len);
		else
		  val_str = ((String_Matrix*)matrix)->FastEl(pos.x, pos.y, opos.x, opos.y).elidedTo(max_txt_len);
	      }
	      else {
		ValToDispText(val, val_str);
	      }
	      txt->string.setValue(val_str.chars());
	      t_idx++;
	    }
	  }
	}
      }
    }
  }

  vertex.finishEditing();
  color.finishEditing();
}


//////////////////////////////////////////////////////////
//		So3DHeightField
/////////////////////////////////////////////////////////

SO_NODE_SOURCE(So3DHeightField);

void So3DHeightField::initClass()
{
  SO_NODE_INIT_CLASS(So3DHeightField, SoSeparator, "SoSeparator");
}

So3DHeightField::So3DHeightField(taMatrix* mat, taMatrix* vec_mat_, ColorScale* sc,
				 MinMaxRange* vec_r, bool vec_nrm_,
				 float pvht, float min_a, float max_a,
				 float vln, float vwdth,
				 bool grid_, bool txt_) {
  SO_NODE_CONSTRUCTOR(So3DHeightField);

  transform_ = new SoTransform;
  addChild(transform_);
  shape_ = new SoIndexedTriangleStripSet;
  addChild(shape_);
  vtx_prop_ = new SoVertexProperty;
  shape_->vertexProperty.setValue(vtx_prop_);
  cell_text_ = NULL;
  cell_fnt_ = NULL;

  grid_sep_ = NULL;
  grid_style_ = NULL;
  grid_mat_ = NULL;
  grid_lines_ = NULL;

  vec_sep_ = NULL;
  vec_style_ = NULL;
  vec_vtx_ = NULL;
  vec_lines_ = NULL;

  vec_norms = vec_nrm_;
  matrix = mat;
  taBase::Ref(matrix);
  vec_matrix = NULL;
  vec_range = NULL;
  if(vec_mat_ && vec_r) {
    vec_matrix = vec_mat_;
    vec_range = vec_r;
    taBase::Ref(vec_matrix);
    setup_vec();
  }
  else if(vec_norms) {
    setup_vec();
  }
  scale = sc;
  plane_vht = pvht;
  min_alpha = min_a;
  max_alpha = max_a;
  vec_len = vln;
  vec_width = vwdth;

  grid_width = 1.0f;
  
  val_text = txt_;
  draw_grid = grid_;
  if(draw_grid)
    setup_grid();
  max_txt_len = 6;
  //  render(); // don't do this by default; often want to spec further guys
}

So3DHeightField::~So3DHeightField() {
  taBase::UnRef(matrix);
  if(vec_matrix)
    taBase::UnRef(vec_matrix);
  matrix = NULL;
  vec_matrix = NULL;
}


void So3DHeightField::setup_grid() {
  grid_sep_ = new SoSeparator();
  grid_style_ = new SoDrawStyle();
  grid_mat_ = new SoMaterial();
  grid_lines_ = new SoIndexedLineSet();
  grid_sep_->addChild(grid_style_);
  grid_sep_->addChild(grid_mat_);
  grid_lines_->vertexProperty.setValue(vtx_prop_); // we use existing ones!!
  grid_sep_->addChild(grid_lines_);
  addChild(grid_sep_);
}

void So3DHeightField::setup_vec() {
  vec_sep_ = new SoSeparator();
  vec_style_ = new SoDrawStyle();
  vec_vtx_ = new SoVertexProperty();
  vec_lines_ = new SoIndexedLineSet();
  vec_sep_->addChild(vec_style_);
  vec_lines_->vertexProperty.setValue(vec_vtx_);
  vec_sep_->addChild(vec_lines_);
  addChild(vec_sep_);
}

void So3DHeightField::setMatrix(taMatrix* mat) { 
  taBase::UnRef(matrix);
  matrix = mat; 
  taBase::Ref(matrix);
  render();
}

void So3DHeightField::setVecMatrix(taMatrix* mat, MinMaxRange* vec_r) { 
  if(vec_matrix)
    taBase::UnRef(matrix);
  else {
    setup_vec();
  }
  vec_matrix = mat; 
  vec_range = vec_r;
  if(vec_matrix)
    taBase::Ref(vec_matrix);
  render();
}

void So3DHeightField::setColorScale(ColorScale* cs) { 
  scale = cs;
  render();
}

void So3DHeightField::setValText(bool val_txt) { 
  val_text = val_txt;
  render();
}

void So3DHeightField::setGrid(bool grid) { 
  draw_grid = grid;
  if(draw_grid && !grid_sep_) {
    setup_grid();
  }
  render();
}

void So3DHeightField::setPlaneVHeight(float vh) { 
  plane_vht = vh;
  render();
}


void So3DHeightField::ValToDispText(float val, String& str) {
  float val_abs = fabsf(val);
  if ((val_abs < .0001) || (val_abs >= 10000))
    str.convert(val, "%7.1e"); //note: 7 chars
  if (val_abs < 1)
    str.convert(val, "%#6.4f");
  else if (val_abs < 10)
    str.convert(val, "%#6.3f");
  else if (val_abs < 100)
    str.convert(val, "%#6.2f");
  else if (val_abs < 1000)
    str.convert(val, "%#6.1f");
  else //must be: if (val_abs < 10000)
    str.convert(val, "6.0f");
}

void So3DHeightField::render() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFVec3f& normal = vtx_prop_->normal;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;

  // this means that the color and normals are always 1-to-1 with the vertex coords
  // system automatically does the proper blending
  vtx_prop_->normalBinding.setValue(SoNormalBinding::PER_VERTEX_INDEXED);
  vtx_prop_->materialBinding.setValue(SoMaterialBinding::PER_VERTEX_INDEXED);

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  // float eff_vht = cl_z * plane_vht;
  
  int n_geom = matrix->count();	// not always x * y due to spaces in display geom
  // adding an extra middle dot between each to make symmetric positions..
  int n_mids = (geom_x-1) * (geom_y-1) * geom_z;

  vertex.setNum(n_geom + n_mids);
  color.setNum(n_geom + n_mids);
  normal.setNum(n_geom + n_mids);

  bool build_text = false;
  float ufontsz = MIN(1.5f * (cl_x / (float)max_txt_len), cl_y);
  if(!val_text) {
    if (cell_text_) {
      removeChild(cell_text_);
      cell_fnt_ = NULL;
      cell_text_ = NULL;
    }
  }
  else {
    if(!cell_text_) {
      cell_text_ = new SoSeparator;
      build_text = true;
      addChild(cell_text_);
      SoBaseColor* bc = new SoBaseColor;
      bc->rgb.setValue(0, 0, 0); //black is default for text
      cell_text_->addChild(bc);
      cell_fnt_ = new SoFont();
      cell_fnt_->name = (const char*)taMisc::t3d_font_name;
      cell_text_->addChild(cell_fnt_);
    }
    cell_fnt_->size.setValue(ufontsz);
  }

  SbVec3f* vertex_dat = vertex.startEditing();

  String val_str;
  T3Color col;
  TDCoord pos;
  int v_idx = 0;
  int t_idx = 2;		// base color + font
  // these go in normal order; indexes are backwards

  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = (float)pos.x * cl_x;
	float yp = ((float)pos.y * cl_y) - 1.0f;
	float zp = (float)pos.z * cl_z;

	vertex_dat[v_idx++].setValue(xp, zp, yp); // z and y switched!!

	if(val_text) {
	  render_text(build_text, t_idx, xp, yp, zp, ufontsz);
	}
      }
    }
  }

  // now do the extra middle dot guys
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y-1; pos.y++) {
      for(pos.x=0; pos.x<geom_x-1; pos.x++) {
	float xp = ((float)pos.x +.5f) * cl_x;
	float yp = (((float)pos.y + .5f) * cl_y) - 1.0f;
	float zp = (float)pos.z * cl_z;

	vertex_dat[v_idx++].setValue(xp, zp, yp); // z and y switched!!
      }
    }
  }
  
  vertex.finishEditing();

  SoMFInt32& coords = shape_->coordIndex;
  // these guys all just derive from coords per vertex indexed
//   SoMFInt32& norms = shape_->normalIndex;
//   SoMFInt32& mats = shape_->materialIndex;
  int nc_per_idx = 10;		// number of coords per index
  coords.setNum(n_mids * nc_per_idx);

  // triangle strip order is 0 1 2, 2 1 3, 2 3 4

  int32_t* coords_dat = coords.startEditing();
  int cidx = 0;
  int nxy = geom_x * geom_y;
  int nxy_mid = (geom_x -1) * (geom_y-1);
  
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=geom_y-2; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x-1; pos.x++) { // left to right
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	int idx_01 = idx_00 + geom_x;
	// middle guy:
	int idx_mid = n_geom + pos.z * nxy_mid + pos.y * (geom_x-1) + pos.x;

	// all around the back
	coords_dat[cidx++] = idx_00; // 1
	coords_dat[cidx++] = idx_01; // 2
	coords_dat[cidx++] = idx_mid; // 3
	coords_dat[cidx++] = idx_01 + 1; // 4
	coords_dat[cidx++] = idx_00 + 1; // 5
	coords_dat[cidx++] = -1; // 6
	// front face:
	coords_dat[cidx++] = idx_00; // 7 
	coords_dat[cidx++] = idx_00 + 1; // 8
	coords_dat[cidx++] = idx_mid; // 9
	coords_dat[cidx++] = -1; // 10
      }
    }
  }
  coords.finishEditing();

  renderGrid();
  renderVector();

  renderValues();		// hand off to next guy..
}

void So3DHeightField::render_text(bool build_text, int& t_idx, float xp, float yp,
				  float zp, float ufontsz)
{
  if(build_text || cell_text_->getNumChildren() <= t_idx) {
    SoSeparator* tsep = new SoSeparator;
    cell_text_->addChild(tsep);
    SoTranslation* tr = new SoTranslation;
    tsep->addChild(tr);
    SoAsciiText* txt = new SoAsciiText();
    txt->justification = SoAsciiText::CENTER;
    tsep->addChild(txt);
  }
  SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
  SoTranslation* tr = (SoTranslation*)tsep->getChild(0);
  float yfp = yp - .5f * ufontsz;
  tr->translation.setValue(xp, MAX(zp,0.0f) + .01f, yfp); // y/z switched
  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
  txt->string.setValue("0.0");	// placeholder; gets set later
  t_idx++;
}

void So3DHeightField::renderValues() {
  if(!matrix || !scale) return;
  if(!shape_ || !vtx_prop_) return; // something wrong..

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  int nxy = geom_x * geom_y;

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  float max_xy = MAX(cl_x, cl_y);

  float eff_vht = cl_z * plane_vht;

  SoMFVec3f& vertex = vtx_prop_->vertex;
  SoMFUInt32& color = vtx_prop_->orderedRGBA;
  SoMFVec3f& normal = vtx_prop_->normal;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();
  SbVec3f* normal_dat = normal.startEditing();

  String val_str;
  float sc_val;
  T3Color col;
  TDCoord pos;
  int v_idx = 0;
  int c_idx = 0;
  int t_idx = 2;		// base color + font

  float max_trans = 1.0f - min_alpha;

  // these go in normal order; indexes are backwards
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float val = matrix->FastElAsFloat(pos.x, pos.y, pos.z);

	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	float zp = sc_val * eff_vht + (float)pos.z * cl_z;
	vertex_dat[v_idx++][1] = zp;

	// sc_val = 1..0.  1- = 0..1  alpha 1 = max opaque
	// old eq:
	// a = mscd.min_a + mscd.max_a * mscd.min_a_c * fabs(val - .5);

	float alpha = max_alpha * (1.0f - ((1.0f - fabsf(sc_val)) * max_trans));
	color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
	if(val_text) {
	  SoSeparator* tsep = (SoSeparator*)cell_text_->getChild(t_idx);
	  SoAsciiText* txt = (SoAsciiText*)tsep->getChild(1);
	  ValToDispText(val, val_str);
	  txt->string.setValue(val_str.chars());
	  SoTranslation* tr = (SoTranslation*)tsep->getChild(0);
	  SbVec3f val = tr->translation.getValue();
	  val[1] = MAX(zp,0.0f) + .01f;
	  tr->translation.setValue(val); // y/z switched
	  t_idx++;
	}
      }
    }
  }

  // now do the extra middle dot guys
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y-1; pos.y++) {
      for(pos.x=0; pos.x<geom_x-1; pos.x++) {
	float val_00 = matrix->FastElAsFloat(pos.x, pos.y, pos.z);
	float val_10 = matrix->FastElAsFloat(pos.x+1, pos.y, pos.z);
	float val_01 = matrix->FastElAsFloat(pos.x, pos.y+1, pos.z);
	float val_11 = matrix->FastElAsFloat(pos.x+1, pos.y+1, pos.z);

	float val = .25f * (val_00 + val_10 + val_01 + val_11);

	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	float zp = sc_val * eff_vht + (float)pos.z * cl_z;
	vertex_dat[v_idx++][1] = zp;
	float alpha = max_alpha * (1.0f - ((1.0f - fabsf(sc_val)) * max_trans));
	color_dat[c_idx++] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
      }
    }
  }

  // normals
  int n_idx = 0;
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float my_zp = vertex_dat[n_idx][1];
	float xm_zp, xp_zp;
	if(pos.x > 0)	  xm_zp = vertex_dat[n_idx-1][1];
	else		  xm_zp = my_zp;
	if(pos.x < geom_x-1) xp_zp = vertex_dat[n_idx+1][1];
	else		     xp_zp = my_zp;

	float ym_zp, yp_zp;
	if(pos.y > 0)	  ym_zp = vertex_dat[n_idx-geom_x][1];
	else		  ym_zp = my_zp;
	if(pos.y < geom_y-1) yp_zp = vertex_dat[n_idx+geom_x][1];
	else		     yp_zp = my_zp;

        float dx = xm_zp - xp_zp;
        float dy = ym_zp - yp_zp;
	
	FloatTDCoord n;
	n.x = dx;	n.y = dy;	n.z = max_xy;
	n.MagNorm();
	normal_dat[n_idx++].setValue(n.x, n.z, n.y); // yz switched
      }
    }
  }

  // now do the extra middle dot guys
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y-1; pos.y++) {
      for(pos.x=0; pos.x<geom_x-1; pos.x++) {
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	int idx_01 = idx_00 + geom_x;

	SbVec3f n_00 = normal_dat[idx_00];
	SbVec3f n_01 = normal_dat[idx_01];
	SbVec3f n_10 = normal_dat[idx_00+1];
	SbVec3f n_11 = normal_dat[idx_01+1];

	SbVec3f n = n_00 + n_01 + n_10 + n_11;
	n *= .25f;
	normal_dat[n_idx++].setValue(n.getValue());
      }
    }
  }

  vertex.finishEditing();
  color.finishEditing();
  normal.finishEditing();

  renderVectorValues();
}

void So3DHeightField::renderGrid() {
  if(!matrix || !scale) return;
  if(!grid_sep_) return;

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

 SoMFInt32& coords = grid_lines_->coordIndex;

  if(!draw_grid) {
    coords.setNum(0);
    return;
  }

  grid_style_->style = SoDrawStyleElement::LINES;
  grid_style_->lineWidth = grid_width;
  grid_mat_->diffuseColor.setValue(0.0f, 0.0f, 0.0f); // black

  int n_coord = (geom_x + 1) * geom_y * geom_z + (geom_y + 1) * geom_x * geom_z;
  coords.setNum(n_coord);

  int32_t* coords_dat = coords.startEditing();
  int cidx = 0;

  int nxy = geom_x * geom_y;
  TDCoord pos;
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    // X guys
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	coords_dat[cidx++] = idx_00;
      }
      coords_dat[cidx++] = -1;	// end that line
    }
    // Y guys
    for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
      for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	coords_dat[cidx++] = idx_00;
      }
      coords_dat[cidx++] = -1;	// end that line
    }
  }

  coords.finishEditing();
}

void So3DHeightField::renderVector() {
  if(!matrix || !scale) return;
  if(!vec_sep_) return;

  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  SoMFVec3f& vertex = vec_vtx_->vertex;
  SoMFUInt32& color = vec_vtx_->orderedRGBA;
  SoMFInt32& coords = vec_lines_->coordIndex;

  vec_vtx_->materialBinding.setValue(SoMaterialBinding::PER_VERTEX_INDEXED);

  if(!vec_norms && (!vec_matrix || !vec_range)) {
    vertex.setNum(0);
    color.setNum(0);
    coords.setNum(0);
    return;
  }

  vec_style_->style = SoDrawStyleElement::LINES;
  vec_style_->lineWidth = vec_width;

  int n_geom = matrix->count();	// not always x * y due to spaces in display geom
  int n_vtx = n_geom*2;		// 2 points per point
  int n_coord = n_geom*3;	// 3 coords per point (-1 term)
  vertex.setNum(n_vtx);
  color.setNum(n_vtx);
  coords.setNum(n_coord);

  SbVec3f* vertex_dat = vertex.startEditing();

  TDCoord pos;
  int v_idx = 0;
  // just standard points
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = (float)pos.x * cl_x;
	float yp = ((float)pos.y * cl_y) - 1.0f;
	float zp = (float)pos.z * cl_z;
	vertex_dat[v_idx++].setValue(xp, zp, yp); // z and y switched!!
      }
    }
  }
  // the rest are all end points and are rendered in values!

  vertex.finishEditing();

  int32_t* coords_dat = coords.startEditing();

  int cidx = 0;
  int nxy = geom_x * geom_y;
  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=geom_y-1; pos.y>=0; pos.y--) { // go back to front
      for(pos.x=0; pos.x<geom_x; pos.x++) { // left to right
	int idx_00 = pos.z * nxy + pos.y * geom_x + pos.x;
	coords_dat[cidx++] = idx_00;
	coords_dat[cidx++] = n_geom + idx_00; // 2nd point in end-space
	coords_dat[cidx++] = -1;	// end that line
      }
    }
  }

  coords.finishEditing();
}

inline static float so3dhf_get_sc_val(float max, float val) {
  float nrm_val = 0.0f;
  if(max > 0.0f)
    nrm_val = val / max;
  return nrm_val;
}

void So3DHeightField::renderVectorValues() {
  if(!matrix || !scale) return;
  if(!vec_sep_) return;

  if(!vec_norms && (!vec_matrix || !vec_range)) {
    return;
  }
  int geom_x = matrix->dim(0);
  int geom_y = matrix->dim(1);
  int geom_z = matrix->dim(2);
  if(geom_x < 2 || geom_y < 2 || geom_z < 1) return;

  float cl_x = 1.0f / (float)(geom_x-1);
  float cl_y = 1.0f / (float)(geom_y-1);
  float cl_z = 1.0f / (float)(geom_z);

  FloatTDCoord vec_ln;
  vec_ln.x = cl_x * vec_len;
  vec_ln.y = cl_y * vec_len;
  vec_ln.z = cl_z * vec_len;

  vec_ln.z = MIN(vec_ln.x, vec_ln.z); // not too big..
  float eff_vht = cl_z * plane_vht;

  int n_geom = matrix->count();	// not always x * y due to spaces in display geom

  SoMFVec3f& vertex = vec_vtx_->vertex;
  SoMFUInt32& color = vec_vtx_->orderedRGBA;
  SoMFVec3f& normal = vtx_prop_->normal;

  SbVec3f* vertex_dat = vertex.startEditing();
  uint32_t* color_dat = color.startEditing();
  
  SbVec3f* normal_dat = NULL;
  if(vec_norms)
    normal_dat = normal.startEditing();

  T3Color col;
  TDCoord pos;
  int v_idx = 0;
  int c_idx = 0;
  float sc_val;
  FloatTDCoord vec;
  FloatTDCoord sp;
  FloatTDCoord ep;

//   float max_trans = 1.0f - min_alpha;

  for(pos.z=0; pos.z<geom_z; pos.z++) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	sp.x = (float)pos.x * cl_x;
	sp.y = ((float)pos.y * cl_y) - 1.0f;
	sp.z = (float)pos.z * cl_z;

	float val = matrix->FastElAsFloat(pos.x, pos.y, pos.z);
	if(vec_norms) {
	  vec.x = so3dhf_get_sc_val(1.0f, normal_dat[v_idx][0]);
	  vec.y = so3dhf_get_sc_val(1.0f, normal_dat[v_idx][2]);
	  vec.z = so3dhf_get_sc_val(1.0f, normal_dat[v_idx][1]);
	}
	else {
	  vec.x = so3dhf_get_sc_val(vec_range->max, vec_matrix->FastElAsFloat(0, pos.x, pos.y, pos.z));
	  vec.y = so3dhf_get_sc_val(vec_range->max, vec_matrix->FastElAsFloat(1, pos.x, pos.y, pos.z));
	  vec.z = so3dhf_get_sc_val(vec_range->max, vec_matrix->FastElAsFloat(2, pos.x, pos.y, pos.z));
	}

	iColor fl;  iColor tx;
	scale->GetColor(val,sc_val,&fl,&tx);
	sp.z += sc_val * eff_vht;
	vertex_dat[v_idx][1] = sp.z;

	ep = sp + vec_ln * vec;
	vertex_dat[n_geom + v_idx].setValue(ep.x, ep.z, ep.y); // xy switch
	v_idx++;

// 	float alpha = max_alpha * (1.0f - ((1.0f - fabsf(sc_val)) * max_trans));
// 	color_dat[c_idx] = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);

	float vec_mag = vec.Mag();
	if(vec_mag > 1.0f) vec_mag = 1.0f;
	if(vec_mag < 0.0f) vec_mag = 0.0f;
	if(vec.Sum() < 0.0f)
	  vec_mag *= -1.0f;
	int clr_idx = (int)((.5f + .5f * vec_mag) * (float)(scale->chunks-1.0f));
	fl = scale->GetColor(clr_idx);

// 	alpha = max_alpha * (1.0f - ((1.0f - fabsf(vec_mag)) * max_trans));
	// just keeping the same color, no transparency..
	float alpha = 1.0f;
	uint32_t clr = T3Color::makePackedRGBA(fl.redf(), fl.greenf(), fl.bluef(), alpha);
 	color_dat[c_idx] = clr;
	color_dat[n_geom + c_idx] = clr;
	
	c_idx++;
      }
    }
  }

  vertex.finishEditing();
  color.finishEditing();
  if(normal_dat)
    normal.finishEditing();
}


//////////////////////////////////////////////////////////////////
//   T3ScaleUniformScaler

SO_NODE_SOURCE(T3ScaleUniformScaler);

void T3ScaleUniformScaler::initClass()
{
  SO_NODE_INIT_CLASS(T3ScaleUniformScaler, SoSeparator, "SoSeparator");
}

T3ScaleUniformScaler::T3ScaleUniformScaler(bool active, float half_size, float cube_size) {
  SO_NODE_CONSTRUCTOR(T3ScaleUniformScaler);

  SoMaterial* mat = new SoMaterial;
  if(active) {
    mat->diffuseColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->emissiveColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->transparency.setValue(drag_activ_clr_tr);
  }
  else {
    mat->diffuseColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->emissiveColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->transparency.setValue(drag_inact_clr_tr);
  }
  addChild(mat);

  float sz = half_size;
  float sz2 = 2.0f * half_size;

  SoCube* cb = new SoCube;
  cb->width = cube_size;
  cb->height = cube_size;
  cb->depth = cube_size;

  SoGroup* pts = new SoGroup;
  addChild(pts);
  float x,y,z;
  for(z=-sz;z<=sz;z+=sz2) {
    for(y=-sz;y<=sz;y+=sz2) {
      for(x=-sz;x<=sz;x+=sz2) {
	SoSeparator* sep = new SoSeparator;
	pts->addChild(sep);
	SoTranslation* trn = new SoTranslation;
	trn->translation.setValue(x,y,z);
	sep->addChild(trn);
	sep->addChild(cb);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////
//   T3TransformBoxRotatorRotator

SO_NODE_SOURCE(T3TransformBoxRotatorRotator);

void T3TransformBoxRotatorRotator::initClass()
{
  SO_NODE_INIT_CLASS(T3TransformBoxRotatorRotator, SoSeparator, "SoSeparator");
}

T3TransformBoxRotatorRotator::T3TransformBoxRotatorRotator(bool active, float half_size, float line_width) {
  SO_NODE_CONSTRUCTOR(T3TransformBoxRotatorRotator);

  SoMaterial* mat = new SoMaterial;
  if(active) {
    mat->diffuseColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->emissiveColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->transparency.setValue(drag_activ_clr_tr);
  }
  else {
    mat->diffuseColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->emissiveColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->transparency.setValue(drag_inact_clr_tr);
  }
  addChild(mat);

  float sz = half_size;
  float sz2 = 2.0f * sz;

  SoCube* cb = new SoCube;
  cb->width = line_width;
  cb->height = sz2;
  cb->depth = line_width;

  SoGroup* pts = new SoGroup;
  addChild(pts);
  float x,z;
  for(z=-sz;z<=sz;z+=sz2) {
    for(x=-sz;x<=sz;x+=sz2) {
      SoSeparator* sep = new SoSeparator;
      pts->addChild(sep);
      SoTranslation* trn = new SoTranslation;
      trn->translation.setValue(x,0.0f,z);
      sep->addChild(trn);
      sep->addChild(cb);
    }
  }
}

//////////////////////////////////////////////////////////////////
//   T3Translate1Translator

SO_NODE_SOURCE(T3Translate1Translator);

void T3Translate1Translator::initClass()
{
  SO_NODE_INIT_CLASS(T3Translate1Translator, SoSeparator, "SoSeparator");
}

T3Translate1Translator::T3Translate1Translator(bool active, float bar_len, float bar_width, float cone_radius, float cone_ht) {
  SO_NODE_CONSTRUCTOR(T3Translate1Translator);

  // all this just for the material!
  SoMaterial* mat = new SoMaterial;
  if(active) {
    mat->diffuseColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->emissiveColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->transparency.setValue(drag_activ_clr_tr);
  }
  else {
    mat->diffuseColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->emissiveColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->transparency.setValue(drag_inact_clr_tr);
  }
  addChild(mat);

  float tr_val = .5f * bar_len + .5f * cone_ht;

  SoCube* cb = new SoCube;
  cb->width = bar_len;
  cb->height = bar_width;
  cb->depth = bar_width;
  addChild(cb);
  
  SoCone* cc = new SoCone;
  cc->height = cone_ht;
  cc->bottomRadius = cone_radius;

  SoSeparator* c1s = new SoSeparator;
  SoTransform* c1t = new SoTransform;
  c1t->translation.setValue(tr_val, 0.0f, 0.0f);
  c1t->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), -1.5708f);
  c1s->addChild(c1t);
  c1s->addChild(cc);
  addChild(c1s);

  SoSeparator* c2s = new SoSeparator;
  SoTransform* c2t = new SoTransform;
  c2t->translation.setValue(-tr_val, 0.0f, 0.0f);
  c2t->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
  c2s->addChild(c2t);
  c2s->addChild(cc);
  addChild(c2s);
}

//////////////////////////////////////////////////////////////////
//   T3Translate2Translator

SO_NODE_SOURCE(T3Translate2Translator);

void T3Translate2Translator::initClass()
{
  SO_NODE_INIT_CLASS(T3Translate2Translator, SoSeparator, "SoSeparator");
}

T3Translate2Translator::T3Translate2Translator(bool active, float bar_len, float bar_width, float cone_radius, float cone_ht) {
  SO_NODE_CONSTRUCTOR(T3Translate2Translator);

  // all this just for the material!
  SoMaterial* mat = new SoMaterial;
  if(active) {
    mat->diffuseColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->emissiveColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->transparency.setValue(drag_activ_clr_tr);
  }
  else {
    mat->diffuseColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->emissiveColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->transparency.setValue(drag_inact_clr_tr);
  }
  addChild(mat);

  float tr_val = .5f * bar_len + .5f * cone_ht;

  SoCube* cbx = new SoCube;
  cbx->width = bar_len;
  cbx->height = bar_width;
  cbx->depth = bar_width;
  addChild(cbx);

  SoCube* cby = new SoCube;
  cby->width = bar_width;
  cby->height = bar_len;
  cby->depth = bar_width;
  addChild(cby);
  
  SoCone* cc = new SoCone;
  cc->height = cone_ht;
  cc->bottomRadius = cone_radius;

  SoSeparator* xc1s = new SoSeparator;
  SoTransform* xc1t = new SoTransform;
  xc1t->translation.setValue(tr_val, 0.0f, 0.0f);
  xc1t->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), -1.5708f);
  xc1s->addChild(xc1t);
  xc1s->addChild(cc);
  addChild(xc1s);

  SoSeparator* xc2s = new SoSeparator;
  SoTransform* xc2t = new SoTransform;
  xc2t->translation.setValue(-tr_val, 0.0f, 0.0f);
  xc2t->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708f);
  xc2s->addChild(xc2t);
  xc2s->addChild(cc);
  addChild(xc2s);

  SoSeparator* yc1s = new SoSeparator;
  SoTransform* yc1t = new SoTransform;
  yc1t->translation.setValue(0.0f, tr_val, 0.0f);
  yc1s->addChild(yc1t);
  yc1s->addChild(cc);
  addChild(yc1s);

  SoSeparator* yc2s = new SoSeparator;
  SoTransform* yc2t = new SoTransform;
  yc2t->translation.setValue(0.0f, -tr_val, 0.0f);
  yc2t->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), (float)PI);
  yc2s->addChild(yc2t);
  yc2s->addChild(cc);
  addChild(yc2s);

}

//////////////////////////////////////////////////////////////////
//   T3TransformBoxDragger

SO_NODE_SOURCE(T3TransformBoxDragger);

void T3TransformBoxDragger::initClass()
{
  SO_NODE_INIT_CLASS(T3TransformBoxDragger, SoSeparator, "SoSeparator");
}

T3TransformBoxDragger::T3TransformBoxDragger(float half_size, float cube_size, float line_width) {
  SO_NODE_CONSTRUCTOR(T3TransformBoxDragger);

  xf_ = new SoTransform;
  addChild(xf_);
  dragger_ = new SoTransformBoxDragger;
  addChild(dragger_);

  // translation slide
  SoSeparator* ts_sep = new SoSeparator;
  SoDrawStyle* ts_ds = new SoDrawStyle;
  ts_ds->style = SoDrawStyle::INVISIBLE;
  ts_sep->addChild(ts_ds);
  SoCoordinate3* ts_c3 = new SoCoordinate3;
  SbVec3f ts_c3_pts[4];
  ts_c3_pts[0].setValue(half_size, half_size, half_size);
  ts_c3_pts[1].setValue(-half_size, half_size, half_size);
  ts_c3_pts[2].setValue(-half_size, -half_size, half_size);
  ts_c3_pts[3].setValue(half_size, -half_size, half_size);
  ts_c3->point.setValues(0, 4, ts_c3_pts);
  ts_sep->addChild(ts_c3);
  SoIndexedFaceSet* ts_fs = new SoIndexedFaceSet;
  int32_t ts_ci[5] = {0, 1, 2, 3, -1};
  ts_fs->coordIndex.setValues(0, 5, ts_ci);
  ts_sep->addChild(ts_fs);

  SoSeparator* ts_act = new SoSeparator;
  ts_act->addChild(ts_sep);
  SoSeparator* ts_inact = new SoSeparator;
  ts_inact->addChild(ts_sep);
  SoSeparator* ts_y = new SoSeparator; // dummy for y axis feedback guy -- not necc
  ts_y->addChild(ts_sep);

  T3TransformBoxRotatorRotator* inact_rot = 
    new T3TransformBoxRotatorRotator(false, half_size, line_width);
  T3TransformBoxRotatorRotator* act_rot = 
    new T3TransformBoxRotatorRotator(true, half_size, line_width);

  // inact is the transparent guy
//   T3Translate2Translator* inact_trans = 
//     new T3Translate2Translator(false, half_size, .5 * line_width, line_width,
// 			       line_width * 2.0f);
  T3Translate2Translator* act_trans = 
    new T3Translate2Translator(true, 4.0 * half_size, .2 * line_width, .5 * line_width,
			       line_width);

  // super-size me so stuff is actually grabable!
  dragger_->setPart("scaler.scaler", new T3ScaleUniformScaler(false, half_size, cube_size));
  dragger_->setPart("rotator1.rotator", inact_rot);
  dragger_->setPart("rotator2.rotator", inact_rot);
  dragger_->setPart("rotator3.rotator", inact_rot);

  dragger_->setPart("scaler.scalerActive", new T3ScaleUniformScaler(true, half_size, cube_size));
  dragger_->setPart("rotator1.rotatorActive", act_rot);
  dragger_->setPart("rotator2.rotatorActive", act_rot);
  dragger_->setPart("rotator3.rotatorActive", act_rot);

  SbString str;
  for (int i = 1; i <= 6; i++) {
    str.sprintf("translator%d", i);
    SoDragger* child = (SoDragger*)dragger_->getPart(str.getString(), FALSE);
    child->setPart("translator", ts_inact);
    child->setPart("translatorActive", ts_act);
    child->setPart("xAxisFeedback", act_trans);
    child->setPart("yAxisFeedback", ts_y); // dummy out; x has everything
  }

  trans_calc_ = new SoCalculator;
  trans_calc_->ref();
  trans_calc_->A.connectFrom(&dragger_->translation);
}

//////////////////////////
//   SoScrollBar
//////////////////////////

SO_NODE_SOURCE(SoScrollBar);

void SoScrollBar::initClass()
{
  SO_NODE_INIT_CLASS(SoScrollBar, SoSeparator, "SoSeparator");
}

void SoScrollBar_DragStartCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  SoScrollBar* sb = (SoScrollBar*)userData;
  sb->DragStartCB(dragger);
}

void SoScrollBar_DraggingCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  SoScrollBar* sb = (SoScrollBar*)userData;
  sb->DraggingCB(dragger);
}

void SoScrollBar_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  SoScrollBar* sb = (SoScrollBar*)userData;
  sb->DragFinishCB(dragger);
}

SoScrollBar::SoScrollBar(int min_, int max_, int val_, int ps_, int ss_, float wdth_,
			 float dpth_) {
  SO_NODE_CONSTRUCTOR(SoScrollBar);

  minimum_ = min_;
  maximum_ = max_;
  value_ = val_;
  pageStep_ = ps_;
  singleStep_ = ss_;
  width_ = wdth_;
  depth_ = dpth_;
  fixValues();

  box_mat_ = new SoMaterial;
  box_mat_->diffuseColor.setValue(frame_clr_r, frame_clr_g, frame_clr_b);
  box_mat_->transparency.setValue(frame_clr_tr);
  addChild(box_mat_);

  box_ = new SoCube;
  box_->width = 1.0f;
  box_->height = width_;
  box_->depth = depth_;
  addChild(box_);

  slide_mat_ = new SoMaterial;
  slide_mat_->diffuseColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
  slide_mat_->emissiveColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
  slide_mat_->transparency.setValue(drag_inact_clr_tr);
  addChild(slide_mat_);

  pos_ = new SoTranslation;
  addChild(pos_);

  slider_sep_ = new SoSeparator;
  slider_tx_ = new SoTransform;
  slider_tx_->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), (float)(.5f * PI));
  slider_sep_->addChild(slider_tx_);
  slider_ = new SoCylinder;
  slider_->radius = .5f * width_;
  slider_->height = sliderSize();
  slider_sep_->addChild(slider_);

  active_sep_ = new SoSeparator;
  
  active_mat_ = new SoMaterial;
  active_mat_->diffuseColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
  active_mat_->emissiveColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
  active_mat_->transparency.setValue(drag_activ_clr_tr);
  active_sep_->addChild(active_mat_);
  active_sep_->addChild(slider_sep_);

  dragger_ = new SoTranslate1Dragger;
  dragger_->setPart("translator", slider_sep_);
  dragger_->setPart("translatorActive", active_sep_);
  dragger_->addFinishCallback(SoScrollBar_DragFinishCB, (void*)this);
  dragger_->addStartCallback(SoScrollBar_DragStartCB, (void*)this);
  dragger_->addMotionCallback(SoScrollBar_DraggingCB, (void*)this);
  addChild(dragger_);

//   pos_->translation.connectFrom(&dragger_->translation);

  valueChanged_cb_ = NULL;
  valueChanged_ud_ = NULL;

  repositionSlider();
}

void SoScrollBar::fixValues() {
  if(minimum_ > maximum_) {
    int tmp = maximum_;
    maximum_ = minimum_;
    minimum_ = tmp;
  }
  //  if(maximum_ == minimum_) maximum_ = minimum_ + 1;

  if(value_>maximum_) value_ = maximum_;
  if(value_<minimum_) value_ = minimum_;

  int range = maximum_ - minimum_;

  if(pageStep_ > range) {
    if(range == 0)
      pageStep_ = 1;
    else
      pageStep_ = range;
  }
}

///////////////////////
//  [         ][    ]
//  Min.....Max..PS..
//  [  doc length   ]
///////////////////////
//  [    PS         ]  (range = 0)


float SoScrollBar::getPos() {
  int range = maximum_ - minimum_;
  float scrng = ((float)range + (float)pageStep_); // scrollable range
  float pos = (float)value_ / scrng;
//   cerr << "getpos: val: " << value_ << " range: " << range << " ps: " << pageStep_
//        << " pos: " << pos << endl;
  return pos;
}

int SoScrollBar::getValFmPos(float pos) {
  int range = maximum_ - minimum_;
  float val = pos * (float)(range + pageStep_); // scrollable range
//   cerr << "getval val: " << val << " range: " << range << " ps: " << pageStep_
//        << " pos: " << pos << endl;
  if(val < 0.0f) val -= .5f;	// effect rounding
  else		 val += .5f;
  return (int)val;
}

float SoScrollBar::sliderSize() {
  int range = maximum_ - minimum_;
  float pspct = (float)pageStep_ / (float)(range + pageStep_);
  return MAX(pspct, width_);	// keep it within range
}

void SoScrollBar::repositionSlider() {
  fixValues();
  float slsz = sliderSize();
  slider_->height = slsz;
  //  pos_->translation.setValue(getPos() - slsz, 0.0f, 0.0f);
//   cerr << "slsz: " << slsz << endl;
  pos_->translation.setValue(getPos()-.5f + .5f * slsz, 0.0f, 0.0f);
}

void SoScrollBar::setValue(int new_val) {
  if(value_ == new_val) return;
  value_ = new_val;
  repositionSlider();
}
void SoScrollBar::setMinimum(int new_min) {
  if(minimum_ == new_min) return;
  minimum_ = new_min;
  repositionSlider();
}
void SoScrollBar::setMaximum(int new_max) {
  if(maximum_ == new_max) return;
  maximum_ = new_max;
  repositionSlider();
}
void SoScrollBar::setPageStep(int new_ps) {
  if(pageStep_ == new_ps) return;
  pageStep_ = new_ps;
  slider_->height = MAX(pageStep_, width_); // don't shrink too far
  repositionSlider();
}
void SoScrollBar::setSingleStep(int new_ss) {
  if(singleStep_ == new_ss) return;
  singleStep_ = new_ss;
}
void SoScrollBar::setWidth(float new_width) {
  if(width_ == new_width) return;
  width_ = new_width;
  box_->height = width_;
  slider_->radius = .5f * width_;
  slider_->height = MAX(pageStep_, width_); // don't shrink too far
}
void SoScrollBar::setDepth(float new_depth) {
  if(depth_ == new_depth) return;
  depth_ = new_depth;
  box_->depth = depth_;
}

void SoScrollBar::setValueChangedCB(SoScrollBarCB cb_fun, void* user_data) {
  valueChanged_cb_ = cb_fun;
  valueChanged_ud_ = user_data;
}

void SoScrollBar::valueChangedCB() {
  if(valueChanged_cb_)
    (*valueChanged_cb_)(this, value_, valueChanged_ud_);
}

// defined in t3viewer.cpp
extern bool so_scrollbar_is_dragging;

void SoScrollBar::DragStartCB(SoTranslate1Dragger* dragger) {
  start_val_ = value_;
  so_scrollbar_is_dragging = true;
}

void SoScrollBar::DraggingCB(SoTranslate1Dragger* dragger) {
  SbVec3f trans = dragger->translation.getValue();
  
  int incr = getValFmPos(trans[0]);

  int nw_val = start_val_ + incr;
  value_ = nw_val;
  fixValues();
  int delta_val = value_ - start_val_;
//   cerr << "trans: " << trans[0] << " incr: " << incr
//        << " nw_val: " << nw_val
//        << endl;
  if(nw_val < minimum_ || nw_val > maximum_) {
    int range = maximum_ - minimum_;
    float nw_pos = (float)delta_val / ((float)range + (float)pageStep_);
    dragger->translation.setValue(nw_pos, 0.0f, 0.0f);
  }
  if(delta_val != 0) {
    valueChangedCB();
  }
}

void SoScrollBar::DragFinishCB(SoTranslate1Dragger* dragger) {
  SbVec3f trans = dragger->translation.getValue();

  int incr = getValFmPos(trans[0]);

//   cerr << "finish trans: " << trans[0] << " incr: " << incr << endl;

  value_ = start_val_ + incr;
  fixValues();

  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  repositionSlider();

  valueChangedCB();
  so_scrollbar_is_dragging = false; // this drops us out of the event loop!!
}

