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


// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

#include "t3node_so.h"

#include <QImage>

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
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTransform.h>
//#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoUnits.h>
#include <Inventor/nodes/SoVertexProperty.h>

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

T3Node::T3Node(void* dataView_)
{
  SO_NODE_CONSTRUCTOR(T3Node);

  dataView = dataView_;

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
  transformCaption(iVec3f(0.1f, 0.0f, 0.05f));
}

void T3Node::setCaption(const char* value) {
  int len = (value) ? strlen(value) : 0;
  // only force getting captionNode if a non-blank value
  SoAsciiText* cn = captionNode(len > 0);
  // note: if node exists, we don't check for same value, so it will trigger a redraw
  // even if it is the same -- this could be changed if desired
  if (cn) {
    cn->string.setValue(value);
    setDefaultCaptionTransform();
  }
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
  if (rotate)
    txfm->rotation.setValue(*rotate);
  if (translate)
    txfm->translation.setValue(SbVec3f(translate->x, translate->y, translate->z + 0.02f)); // offset slightly
}


//////////////////////////
//   T3NodeLeaf		//
//////////////////////////

SO_NODE_SOURCE(T3NodeLeaf);

void T3NodeLeaf::initClass()
{
  SO_NODE_INIT_CLASS(T3NodeLeaf, T3Node, "");
}

T3NodeLeaf::T3NodeLeaf(void* dataView_)
:inherited(dataView_)
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

T3NodeParent::T3NodeParent(void* dataView_)
:inherited(dataView_)
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

SoFrame::SoFrame(Orientation ori) {
  SO_NODE_CONSTRUCTOR(SoFrame);

  vertexProperty.setValue(new SoVertexProperty); // note: vp refs/unrefs automatically
  base = height = 1.0f;
  depth = 0.1f;
  inset = 0.05f;
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
  p.startEditing();
  n.startEditing();
    // bottom
    n.set1Value(j++, 0, -1, 0);
    p.set1Value(i++,-w2,-d2, h2);
    p.set1Value(i++,-(w2-inset),-d2, (h2-inset));
    p.set1Value(i++, w2,-d2, h2);
    p.set1Value(i++, (w2-inset),-d2, (h2-inset));
    p.set1Value(i++, w2,-d2,-h2);
    p.set1Value(i++, (w2-inset),-d2,-(h2-inset));
    p.set1Value(i++,-w2,-d2,-h2);
    p.set1Value(i++,-(w2-inset),-d2,-(h2-inset));
    p.set1Value(i++,-w2,-d2, h2);
    p.set1Value(i++,-(w2-inset),-d2, (h2-inset));

    //outside: l
    n.set1Value(j++, -1, 0, 0);
    p.set1Value(i++,-w2, d2,-h2);
    p.set1Value(i++,-w2,-d2,-h2);
    p.set1Value(i++,-w2, d2, h2);
    p.set1Value(i++,-w2,-d2, h2);

    //outside: bk
    n.set1Value(j++, 0, 0, -1);
    p.set1Value(i++, w2,-d2,-h2);
    p.set1Value(i++,-w2,-d2,-h2);
    p.set1Value(i++, w2, d2,-h2);
    p.set1Value(i++,-w2, d2,-h2);

    //outside: r
    n.set1Value(j++, 1, 0, 0);
    p.set1Value(i++, w2,-d2,-h2);
    p.set1Value(i++, w2, d2,-h2);
    p.set1Value(i++, w2,-d2, h2);
    p.set1Value(i++, w2, d2, h2);

    //outside: fr
    n.set1Value(j++, 0, 0, 1);
    p.set1Value(i++, w2,-d2, h2);
    p.set1Value(i++,-w2,-d2, h2);
    p.set1Value(i++, w2, d2, h2);
    p.set1Value(i++,-w2, d2, h2);

    // top
    n.set1Value(j++, 0, 1, 0);
    p.set1Value(i++,-w2, d2, h2);
    p.set1Value(i++,-(w2-inset), d2, h2-inset);
    p.set1Value(i++,-w2, d2,-h2);
    p.set1Value(i++,-(w2-inset), d2,-(h2-inset));
    p.set1Value(i++, w2, d2,-h2);
    p.set1Value(i++, (w2-inset), d2,-(h2-inset));
    p.set1Value(i++, w2, d2, h2);
    p.set1Value(i++, (w2-inset), d2, (h2-inset));
    p.set1Value(i++,-w2, d2, h2);
    p.set1Value(i++,-(w2-inset), d2, h2-inset);
    // inside
    w2 -= inset;
    h2 -= inset;
    //inside: l
    n.set1Value(j++, 1, 0, 0);
    p.set1Value(i++,-w2, d2,-h2);
    p.set1Value(i++,-w2,-d2,-h2);
    p.set1Value(i++,-w2, d2, h2);
    p.set1Value(i++,-w2,-d2, h2);

    //inside: bk
    n.set1Value(j++, 0, 0, 1);
    p.set1Value(i++, w2,-d2,-h2);
    p.set1Value(i++,-w2,-d2,-h2);
    p.set1Value(i++, w2, d2,-h2);
    p.set1Value(i++,-w2, d2,-h2);

    //inside: r
    n.set1Value(j++, -1, 0, 0);
    p.set1Value(i++, w2,-d2,-h2);
    p.set1Value(i++, w2, d2,-h2);
    p.set1Value(i++, w2,-d2, h2);
    p.set1Value(i++, w2, d2, h2);

    //inside: fr
    n.set1Value(j++, 0, 0, -1);
    p.set1Value(i++, w2,-d2, h2);
    p.set1Value(i++,-w2,-d2, h2);
    p.set1Value(i++, w2, d2, h2);
    p.set1Value(i++,-w2, d2, h2);

  n.finishEditing();
  p.finishEditing();

}

void SoFrame::renderV() {
  //TODO: change all below to draw vertical
  float w2 = base / 2.0f; // x (same as H)
  float d2 = depth / 2.0f; // z
  float h2 = height / 2.0f; // y
  SoMFVec3f& p = vertex();
  SoMFVec3f& n = normal();
  int i = 0; // vertex index
  int j = 0; // normal index
  p.startEditing();
  n.startEditing();
    // back
    n.set1Value(j++, 0, 0, -1);
    p.set1Value(i++,-w2, h2, -d2);
    p.set1Value(i++,-(w2-inset), (h2-inset), -d2);
    p.set1Value(i++, w2, h2, -d2);
    p.set1Value(i++, (w2-inset), (h2-inset), -d2);
    p.set1Value(i++, w2,-h2, -d2);
    p.set1Value(i++, (w2-inset),-(h2-inset), -d2);
    p.set1Value(i++,-w2,-h2, -d2);
    p.set1Value(i++,-(w2-inset),-(h2-inset), -d2);
    p.set1Value(i++,-w2, h2, -d2);
    p.set1Value(i++,-(w2-inset),(h2-inset), -d2);

    //outside: l
    n.set1Value(j++, -1, 0, 0);
    p.set1Value(i++,-w2,-h2,-d2);
    p.set1Value(i++,-w2, h2,-d2);
    p.set1Value(i++,-w2,-h2, d2);
    p.set1Value(i++,-w2, h2, d2);

    //outside: tp
    n.set1Value(j++, 0, 1, 0);
    p.set1Value(i++,-w2, h2,-d2);
    p.set1Value(i++, w2, h2,-d2);
    p.set1Value(i++,-w2, h2, d2);
    p.set1Value(i++, w2, h2, d2);

    //outside: r
    n.set1Value(j++, 1, 0, 0);
    p.set1Value(i++, w2, h2,-d2);
    p.set1Value(i++, w2,-h2,-d2);
    p.set1Value(i++, w2, h2, d2);
    p.set1Value(i++, w2,-h2, d2);

    //outside: bt
    n.set1Value(j++, 0, -1, 0);
    p.set1Value(i++, w2,-h2,-d2);
    p.set1Value(i++,-w2,-h2,-d2);
    p.set1Value(i++, w2,-h2, d2);
    p.set1Value(i++,-w2,-h2, d2);

    // front
    n.set1Value(j++, 0, 0, 1);
    p.set1Value(i++,-w2, h2, d2);
    p.set1Value(i++,-(w2-inset), (h2-inset), d2);
    p.set1Value(i++, w2, h2, d2);
    p.set1Value(i++, (w2-inset), (h2-inset), d2);
    p.set1Value(i++, w2,-h2, d2);
    p.set1Value(i++, (w2-inset),-(h2-inset), d2);
    p.set1Value(i++,-w2,-h2, d2);
    p.set1Value(i++,-(w2-inset),-(h2-inset), d2);
    p.set1Value(i++,-w2, h2, d2);
    p.set1Value(i++,-(w2-inset),(h2-inset), d2);

    w2 -= inset;
    h2 -= inset;
    //inside: l
    n.set1Value(j++, 1, 0, 0);
    p.set1Value(i++,-w2,-h2,-d2);
    p.set1Value(i++,-w2, h2,-d2);
    p.set1Value(i++,-w2,-h2, d2);
    p.set1Value(i++,-w2, h2, d2);

    //inside: tp
    n.set1Value(j++, 0, -1, 0);
    p.set1Value(i++,-w2, h2,-d2);
    p.set1Value(i++, w2, h2,-d2);
    p.set1Value(i++,-w2, h2, d2);
    p.set1Value(i++, w2, h2, d2);

    //inside: r
    n.set1Value(j++, -1, 0, 0);
    p.set1Value(i++, w2, h2,-d2);
    p.set1Value(i++, w2,-h2,-d2);
    p.set1Value(i++, w2, h2, d2);
    p.set1Value(i++, w2,-h2, d2);

    //inside: bt
    n.set1Value(j++, 0, 1, 0);
    p.set1Value(i++, w2,-h2,-d2);
    p.set1Value(i++,-w2,-h2,-d2);
    p.set1Value(i++, w2,-h2, d2);
    p.set1Value(i++,-w2,-h2, d2);


  n.finishEditing();
  p.finishEditing();

}

void SoFrame::setDimensions(float bs, float ht, float dp, float in) {
  if ((base == bs) && (height == ht) && (depth == dp) && (inset == in)) return;
  base = bs;  height = ht;  depth = dp;  inset = in;
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
//   SoImageEx		//
//////////////////////////

SO_NODE_SOURCE(SoImageEx);

void SoImageEx::initClass()
{
  SO_NODE_INIT_CLASS(SoImageEx, SoImage, "SoImage");
}

SoImageEx::SoImageEx() {
  SO_NODE_CONSTRUCTOR(SoImageEx);
}

void SoImageEx::setImage(const QImage& src) {
  if (src.isGrayscale())
    setImage2(src);
  else 
    setImage3(src);
}

void SoImageEx::setImage(const float_Matrix& src) {
  int dims = src.dims(); //cache
  if (dims == 2) {
    setImage2(src);
  } else if (((dims == 3) && (src.dim(2) == 3))) { 
    setImage3(src);
  } else { 
    taMisc::Error("SoImageEx::setImage: must be grey or rgb matrix");
    return;
  }
}

void SoImageEx::setImage2(const float_Matrix& src) {
  int dx = src.dim(0);
  int dy = src.dim(1);
  img.SetGeom(2, dx, dy);
  int idx = 0;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = dy - 1; y >= 0; --y) {
    for (int x = 0; x < dx; ++x) {
      img.FastEl_Flat(idx) = (byte)(src.FastEl(x, y) * 255);
      ++idx;
    }
  }
  image.setValue(SbVec2s(dx, dy), 1, (const unsigned char*)img.data());
}

void SoImageEx::setImage2(const QImage& src) {
  int dx = src.width();
  int dy = src.height();
  img.SetGeom(2, dx, dy);
  int idx = 0;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = dy - 1; y >= 0; --y) {
    for (int x = 0; x < dx; ++x) {
      img.FastEl_Flat(idx) = (byte)(qGray(src.pixel(x, y)));
      ++idx;
    }
  }
  image.setValue(SbVec2s(dx, dy), 1, (const unsigned char*)img.data());
}

void SoImageEx::setImage3(const float_Matrix& src) {
  int dx = src.dim(0);
  int dy = src.dim(1);
  //NOTE: img geom is not same as input: rgb is in innermost for us
  img.SetGeom(3, 3, dx, dy);
  int idx = 0;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = dy - 1; y >= 0; --y) {
    for (int x = 0; x < dx; ++x) {
      img.FastEl_Flat(idx++) = (byte)(src.FastEl(x, y, 0) * 255);
      img.FastEl_Flat(idx++) = (byte)(src.FastEl(x, y, 1) * 255);
      img.FastEl_Flat(idx++) = (byte)(src.FastEl(x, y, 2) * 255);
    }
  }
  image.setValue(SbVec2s(dx, dy), 3, (const unsigned char*)img.data());
}

void SoImageEx::setImage3(const QImage& src) {
  int dx = src.width();
  int dy = src.height();
  //NOTE: img geom is not same as input: rgb is in innermost for us
  img.SetGeom(3, 3, dx, dy);
  int idx = 0;
  QRgb rgb;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = dy - 1; y >= 0; --y) {
    for (int x = 0; x < dx; ++x) {
    rgb = src.pixel(x, y);
      img.FastEl_Flat(idx++) = (byte)(qRed(rgb));
      img.FastEl_Flat(idx++) = (byte)(qGreen(rgb));
      img.FastEl_Flat(idx++) = (byte)(qBlue(rgb));
    }
  }
  image.setValue(SbVec2s(dx, dy), 3, (const unsigned char*)img.data());
}

