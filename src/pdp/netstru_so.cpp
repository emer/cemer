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



// stuff to implement unit view..

#include "netstru_so.h"

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
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
//#include <Inventor/nodes/SoPerspectiveCamera.h>
//#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

#include <math.h>
#include <limits.h>

//////////////////////////////////
//	  T3UnitNode		//
//////////////////////////////////

#define T3_UNIT_BASE_H	0.1f
#define T3_UNIT_MAX_H	0.8f

SO_NODE_ABSTRACT_SOURCE(T3UnitNode);

float T3UnitNode::base_height = 0.08f;
float T3UnitNode::max_height = 0.46f;

void T3UnitNode::initClass()
{
  SO_NODE_INIT_ABSTRACT_CLASS(T3UnitNode, T3NodeLeaf, "T3NodeLeaf");
}

T3UnitNode::T3UnitNode(void* dataView_, float max_x, float max_y, float max_z, float un_spc)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode);
  spacing = un_spc;
}

T3UnitNode::~T3UnitNode()
{
}

void T3UnitNode::setAppearance(float act, const T3Color& color, float max_z, float trans) {
  material()->diffuseColor = (SbColor)color;
  //  material()->specularColor = (SbColor)color;
  //  material()->emissiveColor = (SbColor)color;
  //  material()->ambientColor = (SbColor)color;
  if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  material()->transparency = (1.0f - fabsf(act)) * trans;
}

void T3UnitNode::setDefaultCaptionTransform() {
  //note: this is the one for 3d objects -- 2d replace this
  captionNode_->justification = SoAsciiText::CENTER;
  //  transformCaption(SbVec3f(0.0f, 0.1f, 0.45f));
  transformCaption(SbVec3f(0.0f, 0.0f, 0.0f));
}

void T3UnitNode::setPicked(bool value) {
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
}


//////////////////////////
//   T3UnitNode_Cylinder//
//////////////////////////

SO_NODE_SOURCE(T3UnitNode_Cylinder);

void T3UnitNode_Cylinder::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Cylinder, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Cylinder::T3UnitNode_Cylinder(void* dataView_, float max_x, float max_y,
					 float max_z, float un_spc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Cylinder);

  float max_xy = MAX(max_x, max_y);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = (.5f - spacing) / max_xy;
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Cylinder::~T3UnitNode_Cylinder()
{
  shape_ = NULL;
}

void T3UnitNode_Cylinder::setAppearance(float act, const T3Color& color, float max_z,
					float trans) {
  if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  shape_->height = (base_height + ((max_height - base_height) * fabs(act))) / max_z;
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = (((max_height - base_height) * act) * 0.5f) / max_z;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance(act, color, max_z, trans);
}

SO_NODE_SOURCE(T3UnitNode_Circle);

void T3UnitNode_Circle::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Circle, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Circle::T3UnitNode_Circle(void* dataView_, float max_x, float max_y,
				     float max_z, float un_spc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Circle);

  float max_xy = MAX(max_x, max_y);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = (.5f - spacing) / max_xy; // always the same radius
  shape_->height = 0.01f;
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Circle::~T3UnitNode_Circle()
{
  shape_ = NULL;
}

/*void T3UnitNode_Circle::setDefaultCaptionTransform() {
  //note: this is the one for 3d objects -- 2d replace this
  captionNode_->justification = SoAsciiText::CENTER;
  transformCaption(SbRotation(SbVec3f(1, 0, 0), (.5 * PI)));
} */


//////////////////////////
//   T3UnitNode_Block//
//////////////////////////

// note: Block is never called now because it is done in optimized form by the unit group

SO_NODE_SOURCE(T3UnitNode_Block);

void T3UnitNode_Block::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Block, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Block::T3UnitNode_Block(void* dataView_, float max_x, float max_y,
				   float max_z, float un_spc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Block);

  shape_ = new SoCube; // note: same shape is used for both styles
  shape_->width = (1.0f - spacing) / max_x; 
  shape_->depth = (1.0f - spacing) / max_y; 
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .0f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Block::~T3UnitNode_Block()
{
  shape_ = NULL;
}

void T3UnitNode_Block::setAppearance(float act, const T3Color& color, float max_z,
				     float trans) {
  if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  shape_->height = (base_height + ((max_height - base_height) * fabs(act))) / max_z;
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = (((max_height - base_height) * act) * 0.5f) / max_z;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance(act, color, max_z, trans);
}

//////////////////////////
//   T3UnitNode_Rect//
//////////////////////////

SO_NODE_SOURCE(T3UnitNode_Rect);

void T3UnitNode_Rect::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Rect, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Rect::T3UnitNode_Rect(void* dataView_, float max_x, float max_y, float max_z,
				 float un_spc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Rect);

  shape_ = new SoCube; // note: same shape is used for both styles
  shape_->width = (1.0f - spacing) / max_x;
  shape_->depth = (1.0f - spacing) / max_y;
  shape_->height = 0.01f;
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Rect::~T3UnitNode_Rect()
{
  shape_ = NULL;
}

//////////////////////////
//   T3UnitGroupNode	//
//////////////////////////

/*
      childNodes: SoSeparator -- the T3Node children (from parent class)
      units: SoGroup -- group for convenience to hold all the T3UnitNode objects
*/
float T3UnitGroupNode::height = 0.0f;
float T3UnitGroupNode::inset = 0.05f;

SO_NODE_SOURCE(T3UnitGroupNode);

void T3UnitGroupNode::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitGroupNode, T3NodeParent, "T3NodeParent");
}

void T3UnitGroupNode::shapeCallback(void* data, SoAction* act) {
  T3UnitGroupNode* node = (T3UnitGroupNode*)data;
  if (act->isOfType(SoGLRenderAction::getClassTypeId())) {
    if(!node->no_units)
      drawGrid(node);
  }
}

void T3UnitGroupNode::drawGrid(T3UnitGroupNode* node) {
  float sw = 0.02f; // strip width
  float x_end = (float)node->geom.x / node->max_size.x;
  float y_end = (float)(-node->geom.y) / node->max_size.y;
  GLbitfield attribs = (GLbitfield)(GL_LIGHTING_BIT | GL_TRANSFORM_BIT);
  glPushMatrix();
  glPushAttrib(attribs); //note: doesn't seem to push matrix properly
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  glDisable(GL_LIGHTING);
  glColor3f(0.4f, 0.4f, 0.4f);
  // vert lines
  for (int x = 1; x < node->geom.x; ++x) {
    glRectf((float)(x - sw) / node->max_size.x, 0.0f,
	    (float)(x + sw) / node->max_size.x, y_end);
  }
  // hor lines
  for (int y = 1; y < node->geom.y; ++y) {
    glRectf(0.0f, (float)-(y - sw) / node->max_size.y,
	    x_end, (float)-(y + sw) / node->max_size.y);
  }
  glPopAttrib();
  glPopMatrix();
}

T3UnitGroupNode::T3UnitGroupNode(void* dataView_, bool no_unts)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitGroupNode);
  unitCaptionFont_ = NULL;
  SoSeparator* ss = shapeSeparator();

  no_units = no_unts;
  if(no_units) {
    shape_ = new SoIndexedTriangleStripSet;
    ss->addChild(shape_);
  }
  else {
    SoCallback* cb = new SoCallback();
    cb->setCallback(shapeCallback, (void*)this);
    insertChildAfter(topSeparator(), cb, transform());
  }
  unit_text_ = NULL;
}

T3UnitGroupNode::~T3UnitGroupNode()
{
  shape_ = NULL;
  unit_text_ = NULL;
  SoMaterial* mat = material();
  mat->diffuseColor.setValue(1.0f, 1.0f, 1.0f); // white (invisible)
  mat->transparency.setValue(1.0f);
}

void T3UnitGroupNode::setGeom(int x, int y, float max_x, float max_y, float max_z) {
  if (geom.isEqual(x, y)) return; // nothing to do, not changed
  geom.setValue(x, y);
  max_size.setValue(max_x, max_y, max_z);
}

SoFont* T3UnitGroupNode::unitCaptionFont(bool auto_create) {
  if (unitCaptionFont_ || !auto_create) return unitCaptionFont_;
  if (!unitCaptionFont_) {
    unitCaptionFont_ = new SoFont();
    insertChildBefore(topSeparator(), unitCaptionFont_, childNodes());
  }
  return unitCaptionFont_;
}

SoSeparator* T3UnitGroupNode::getUnitText() {
  if(unit_text_) return unit_text_;
  unit_text_ = new SoSeparator;
  return unit_text_;
}

SoSeparator* T3UnitGroupNode::removeUnitText() {
  if(unit_text_)
    removeChild(unit_text_);
  unit_text_ = NULL;
}

//////////////////////////
//   T3LayerNode	//
//////////////////////////

float T3LayerNode::height = 0.05f; // height of the layer shape itself

SO_NODE_SOURCE(T3LayerNode);

void T3LayerNode::initClass()
{
  SO_NODE_INIT_CLASS(T3LayerNode, T3NodeParent, "T3NodeParent");
}

T3LayerNode::T3LayerNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3LayerNode);

  SoSeparator* ss = shapeSeparator(); // cache

  shape_ = new SoFrame();
  ss->addChild(shape_);
}

T3LayerNode::~T3LayerNode()
{
//  shape_ = NULL;
}

void T3LayerNode::render() {
  float fx = (float)geom.x / max_size.x;
  float fy = (float)geom.y / max_size.y;
  float max_xy = MAX(max_size.x, max_size.y);
  shape_->setDimensions(fx, fy, 0.05f / max_xy, -0.25f / max_xy);
  // note: LayerView already translates us up into vertical center of cell
  txfm_shape()->translation.setValue(fx/2.0f, 0.0f, -fy/2.0f);
}

void T3LayerNode::setGeom(int x, int y, float max_x, float max_y, float max_z) {
  if (geom.isEqual(x, y)) return; // nothing to do, not changed
  geom.setValue(x, y);
  max_size.setValue(max_x, max_y, max_z);
  render();
}


//////////////////////////////////
//	  T3PrjnNode		//
//////////////////////////////////

SO_NODE_SOURCE(T3PrjnNode);

void T3PrjnNode::initClass()
{
  SO_NODE_INIT_CLASS(T3PrjnNode, T3NodeParent, "T3NodeParent");
}

T3PrjnNode::T3PrjnNode(void* dataView_, float rad)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3PrjnNode);
  radius = rad;
  init();
}

T3PrjnNode::~T3PrjnNode()
{
  trln_prjn = NULL;
  rot_prjn = NULL;
  trln_arr = NULL;
  arr_prjn = NULL;
  line_prjn = NULL;
}

void T3PrjnNode::init() {
  SoSeparator* ss = shapeSeparator();

  rot_prjn = new SoTransform();
  ss->addChild(rot_prjn);
  trln_prjn = new SoTransform();
  ss->addChild(trln_prjn);

  line_prjn = new SoCylinder();
  line_prjn->radius = radius; //height is variable, depends on length
  ss->addChild(line_prjn);

  trln_arr = new SoTransform();
  ss->addChild(trln_arr);

  arr_prjn = new SoCone();
  arr_prjn->height =  3.0f * radius;
  arr_prjn->bottomRadius = 2.0f * radius;
  ss->addChild(arr_prjn);
}

void T3PrjnNode::setEndPoint(const SbVec3f& ep) {
  float dist = ep.length();

  line_prjn->height = dist;
  // txfm
  rot_prjn->rotation.setValue(SbRotation(SbVec3f(0, 1.0f, 0), ep));
  trln_prjn->translation.setValue(0.0f, line_prjn->height.getValue() / 2.0f, 0.0f);
  trln_arr->translation.setValue(0.0f, line_prjn->height.getValue() / 2.0f, 0.0f); //note: already txlted by 1/2 height
}

//////////////////////////
//   T3NetNode		//
//////////////////////////


SO_NODE_SOURCE(T3NetNode);

void T3NetNode::initClass()
{
  SO_NODE_INIT_CLASS(T3NetNode, T3NodeParent, "T3NodeParent");
}

T3NetNode::T3NetNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3NetNode);

  //  shape_ = new SoCube;
  shape_ = new SoFrame();
//   shape_->setName("shape");
  shapeSeparator()->addChild(shape_);

  float h = 0.04f; // nominal amount of height, so we don't vanish
  float x = 1.0f;
  float y = 1.0f;
  // set size/pos of cube -- move down to -1 y
  txfm_shape()->translation.setValue(x/2.0f, h/2.0f - .5f, -y/2.0f);
  shape_->setDimensions(x, y, 0.02f, -0.02f);
  net_text_ = NULL;
}

T3NetNode::~T3NetNode()
{
}

void T3NetNode::setDefaultCaptionTransform() {
  SbVec3f tran(0.0f, -.5f, 0.01f);
  transformCaption(tran);
}

SoSeparator* T3NetNode::getNetText() {
  if(net_text_) return net_text_;
  net_text_ = new SoSeparator;
  return net_text_;
}
