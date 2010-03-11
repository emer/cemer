// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
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
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCylinder.h>
//#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
//#include <Inventor/nodes/SoPerspectiveCamera.h>
//#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodes/SoIndexedLineSet.h>

#include <math.h>
#include <limits.h>
#include <float.h>

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

T3UnitNode::T3UnitNode(void* dataView_, float max_x, float max_y, float max_z,
		       float un_spc, float disp_sc)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode);
  spacing = un_spc;
  disp_scale = disp_sc;
}

T3UnitNode::~T3UnitNode()
{
}

void T3UnitNode::setAppearance(float act, const T3Color& color, float max_z, float trans) {
  bool act_invalid = false;
  if (isnan(act) || isinf(act)) {
    act_invalid = true;
    act = 0.0f;
  }
  else if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  setAppearance_impl(act, color, max_z, trans, act_invalid);
}

void T3UnitNode::setAppearance_impl(float act, const T3Color& color,
  float max_z, float trans, bool act_invalid) 
{
  material()->diffuseColor = (SbColor)color;
  //  material()->specularColor = (SbColor)color;
  //  material()->emissiveColor = (SbColor)color;
  //  material()->ambientColor = (SbColor)color;
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
					 float max_z, float un_spc, float disp_sc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc, disp_sc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Cylinder);

  float max_xy = MAX(max_x, max_y);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = disp_scale * ((.5f - spacing) / max_xy);
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Cylinder::~T3UnitNode_Cylinder()
{
  shape_ = NULL;
}

void T3UnitNode_Cylinder::setAppearance_impl(float act, const T3Color& color, float max_z,
  float trans, bool act_invalid) 
{
  shape_->height = (base_height + ((max_height - base_height) * fabs(act))) / max_z;
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = (((max_height - base_height) * act) * 0.5f) / max_z;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance_impl(act, color, max_z, trans, act_invalid);
}

SO_NODE_SOURCE(T3UnitNode_Circle);

void T3UnitNode_Circle::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Circle, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Circle::T3UnitNode_Circle(void* dataView_, float max_x, float max_y,
				     float max_z, float un_spc, float disp_sc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc, disp_sc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Circle);

  float max_xy = MAX(max_x, max_y);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = disp_scale * ((.5f - spacing) / max_xy); // always the same radius
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
				   float max_z, float un_spc, float disp_sc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc, disp_sc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Block);

  shape_ = new SoCube; // note: same shape is used for both styles
  shape_->width = disp_scale * ((1.0f - spacing) / max_x); 
  shape_->depth = disp_scale * ((1.0f - spacing) / max_y); 
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .0f), 100.0, 0.0f); //dk gray
}

T3UnitNode_Block::~T3UnitNode_Block()
{
  shape_ = NULL;
}

void T3UnitNode_Block::setAppearance_impl(float act, const T3Color& color, float max_z,
  float trans, bool act_invalid) 
{
  shape_->height = (base_height + ((max_height - base_height) * fabs(act))) / max_z;
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = (((max_height - base_height) * act) * 0.5f) / max_z;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance_impl(act, color, max_z, trans, act_invalid);
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
				 float un_spc, float disp_sc)
  :inherited(dataView_, max_x, max_y, max_z, un_spc, disp_sc)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Rect);

  shape_ = new SoCube; // note: same shape is used for both styles
  shape_->width = disp_scale * ((1.0f - spacing) / max_x);
  shape_->depth = disp_scale * ((1.0f - spacing) / max_y);
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
  float disp_scale = node->disp_scale;
  float x_end = disp_scale * ((float)node->geom.x / node->max_size.x);
  float y_end = disp_scale * ((float)(-node->geom.y) / node->max_size.y);
  GLbitfield attribs = (GLbitfield)(GL_LIGHTING_BIT | GL_TRANSFORM_BIT);
  glPushMatrix();
  glPushAttrib(attribs); //note: doesn't seem to push matrix properly
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  glDisable(GL_LIGHTING);
  glColor3f(0.4f, 0.4f, 0.4f);
  // vert lines
  for (int x = 1; x < node->geom.x; ++x) {
    glRectf((disp_scale * (float)(x - sw)) / node->max_size.x, 0.0f,
	    (disp_scale * (float)(x + sw)) / node->max_size.x, y_end);
  }
  // hor lines
  for (int y = 1; y < node->geom.y; ++y) {
    glRectf(0.0f, (disp_scale * (float)-(y - sw)) / node->max_size.y,
	    x_end, (disp_scale * (float)-(y + sw)) / node->max_size.y);
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
    vtx_prop_ = new SoVertexProperty;
    shape_->vertexProperty.setValue(vtx_prop_); // note: vp refs/unrefs automatically
    ss->addChild(shape_);
  }
  else {
    shape_ = NULL;
    vtx_prop_ = NULL;
    SoCallback* cb = new SoCallback();
    cb->setCallback(shapeCallback, (void*)this);
    insertChildAfter(topSeparator(), cb, transform());
  }
  unit_text_ = NULL;

  snap_bord_ = new SoSeparator;
  //  snap_bord_->setName("SnapBord");
  snap_bord_draw_ = new SoDrawStyle;
  snap_bord_->addChild(snap_bord_draw_);
  snap_bord_set_ = new SoIndexedLineSet;
  snap_bord_vtx_prop_ = new SoVertexProperty;
  snap_bord_set_->vertexProperty.setValue(snap_bord_vtx_prop_); // does ref/unref
  snap_bord_->addChild(snap_bord_set_);
  ss->addChild(snap_bord_);
}

T3UnitGroupNode::~T3UnitGroupNode()
{
  shape_ = NULL;
  unit_text_ = NULL;
  SoMaterial* mat = material();
  mat->diffuseColor.setValue(1.0f, 1.0f, 1.0f); // white (invisible)
  mat->transparency.setValue(1.0f);
}

void T3UnitGroupNode::setGeom(int x, int y, float max_x, float max_y, float max_z,
			      float disp_sc) {
  //  if (geom.isEqual(x, y)) return; // nothing to do, not changed
  geom.setValue(x, y);
  max_size.setValue(max_x, max_y, max_z);
  disp_scale = disp_sc;
  scaled_geom.setValue((int)ceil(disp_scale * (float)x), (int)ceil(disp_scale * (float)y));
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

void T3UnitGroupNode::removeUnitText() {
  if(unit_text_)
    removeChild(unit_text_);
  unit_text_ = NULL;
}


////////////////////////////////////////////////////
//   T3LayerNode

const float T3LayerNode::height = 0.05f;
const float T3LayerNode::width = 0.5f;
const float T3LayerNode::max_width = 0.05f;

SO_NODE_SOURCE(T3LayerNode);

void T3LayerNode::initClass()
{
  SO_NODE_INIT_CLASS(T3LayerNode, T3NodeParent, "T3NodeParent");
}

extern void T3LayerNode_XYDragFinishCB(void* userData, SoDragger* dragger);
extern void T3LayerNode_ZDragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

T3LayerNode::T3LayerNode(void* dataView_, bool show_draggers)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3LayerNode);

  show_drag_ = show_draggers;

  if(show_drag_) {
    const float len = .08f;	// bar_len
    const float wd = .1f * len;	// bar_width
    const float cr = .2f * len;	// cone radius
    const float ch = .4f * len;	// cone height

    // XY dragger
    xy_drag_sep_ = new SoSeparator;
    xy_drag_xf_ = new SoTransform;
    xy_drag_xf_->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), -1.5707963f);
    xy_drag_sep_->addChild(xy_drag_xf_);
    xy_dragger_ = new SoTranslate2Dragger;
    xy_dragger_->setPart("translator", new T3Translate2Translator(false, len, wd, cr, ch));
    xy_dragger_->setPart("translatorActive", new T3Translate2Translator(true, len, wd, cr, ch));
    xy_drag_sep_->addChild(xy_dragger_);

    topSeparator()->addChild(xy_drag_sep_);

    // A = XY
    xy_drag_calc_ = new SoCalculator;
    xy_drag_calc_->ref();
    xy_drag_calc_->A.connectFrom(&xy_dragger_->translation);

    xy_dragger_->addFinishCallback(T3LayerNode_XYDragFinishCB, (void*)this);

    // Z dragger
    z_drag_sep_ = new SoSeparator;
    z_drag_xf_ = new SoTransform;
    z_drag_xf_->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5707963f);
    z_drag_sep_->addChild(z_drag_xf_);
    z_dragger_ = new SoTranslate1Dragger;
    z_dragger_->setPart("translator", new T3Translate1Translator(false, len, wd, cr, ch));
    z_dragger_->setPart("translatorActive", new T3Translate1Translator(true, len, wd, cr, ch));
    z_drag_sep_->addChild(z_dragger_);

    topSeparator()->addChild(z_drag_sep_);

    // B = Z
    xy_drag_calc_->B.connectFrom(&z_dragger_->translation);
    //    xy_drag_calc_->expression = "oA = vec3f(.5 + A[0], B[0], -(.5 + A[1]))";
    txfm_shape()->translation.connectFrom(&xy_drag_calc_->oA);

    z_dragger_->addFinishCallback(T3LayerNode_ZDragFinishCB, (void*)this);
  }
  else {
    xy_drag_sep_ = NULL;
    xy_drag_xf_ = NULL;
    xy_dragger_ = NULL;
    xy_drag_calc_ = NULL;

    z_drag_sep_ = NULL;
    z_drag_xf_ = NULL;
    z_dragger_ = NULL;
    z_drag_calc_ = NULL;
  }

  SoSeparator* ss = shapeSeparator(); // cache
  shape_ = new SoFrame();
  ss->addChild(shape_);
}

T3LayerNode::~T3LayerNode()
{
//  shape_ = NULL;
}

void T3LayerNode::render() {
  float fx = disp_scale * ((float)geom.x / max_size.x);
  float fy = disp_scale * ((float)geom.y / max_size.y);
  float max_xy = MAX(max_size.x, max_size.y);
  float lay_wd = width / max_xy;
  lay_wd = MIN(lay_wd, max_width);
  
  float xfrac = .5f * fx;
  float yfrac = .5f * fy;

  shape_->setDimensions(fx, fy, height / max_xy, -lay_wd);
  // note: LayerView already translates us up into vertical center of cell
  txfm_shape()->translation.setValue(xfrac, 0.0f, -yfrac);

  if(show_drag_) {
    float len = .08f;	// bar_len
    len = MIN(len, .5f * fx);

    float wd = .1f * len;	// bar_width
    float cr = .2f * len;	// cone radius
    float ch = .4f * len;	// cone height

    String expr = "oA = vec3f(" + String(xfrac) + " + A[0], B[0], -(" + String(yfrac) + " + A[1]))";
    xy_drag_calc_->expression = expr.chars();

    if(len != .08f) {
      xy_dragger_->setPart("translator", new T3Translate2Translator(false, len, wd, cr, ch));
      xy_dragger_->setPart("translatorActive", new T3Translate2Translator(true, len, wd, cr, ch));
      z_dragger_->setPart("translator", new T3Translate1Translator(false, len, wd, cr, ch));
      z_dragger_->setPart("translatorActive", new T3Translate1Translator(true, len, wd, cr, ch));
    }
  }
}

void T3LayerNode::setGeom(int x, int y, float max_x, float max_y, float max_z,
			  float disp_sc) {
//   if (geom.isEqual(x, y)) return; // nothing to do, not changed
  geom.setValue(x, y);
  max_size.setValue(max_x, max_y, max_z);
  disp_scale = disp_sc;
  scaled_geom.setValue((int)ceil(disp_scale * (float)x), (int)ceil(disp_scale * (float)y));
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

T3PrjnNode::T3PrjnNode(void* dataView_, bool proj, float rad)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3PrjnNode);
  projected_ = proj;
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

  complexity = new SoComplexity;
  //  complexit->type = SoComplexity::SCREEN_SPACE;
  complexity->value.setValue(.1f); // don't need much..
  ss->addChild(complexity);

  rot_prjn = new SoTransform();
  ss->addChild(rot_prjn);
  trln_prjn = new SoTransform();
  ss->addChild(trln_prjn);

  line_prjn = new SoCylinder();
  line_prjn->radius = radius; //height is variable, depends on length
  ss->addChild(line_prjn);

  trln_arr = new SoTransform();
  ss->addChild(trln_arr);

  arr_mat = new SoMaterial;
  // overwritten in qtso..
  arr_mat->diffuseColor.setValue(SbColor(1.0f, .8f, 0.0f)); // orange
  if(projected_)
    arr_mat->transparency.setValue(.5f);
  else
    arr_mat->transparency.setValue(.8f);
  ss->addChild(arr_mat);

  arr_prjn = new SoCone();
  arr_prjn->height =  6.0f * radius;
  arr_prjn->bottomRadius = 4.0f * radius;
  ss->addChild(arr_prjn);
}

void T3PrjnNode::setEndPoint(const SbVec3f& ep) {
  float len = ep.length() - 6.0f * radius;
  if(len < radius) len = radius;

  float hlf_len = .5f *len;

  line_prjn->height = len;
  // txfm
  rot_prjn->rotation.setValue(SbRotation(SbVec3f(0, 1.0f, 0), ep));
  trln_prjn->translation.setValue(0.0f, hlf_len, 0.0f);
  trln_arr->translation.setValue(0.0f, hlf_len + 3.0f * radius, 0.0f);
  //note: already txlted by 1/2 height -- adds arrow len offset
}

void T3PrjnNode::setArrowColor(const SbColor& clr, float transp) {
  arr_mat->diffuseColor.setValue(clr);
  arr_mat->transparency.setValue(transp);
}


////////////////////////////////////////////////////
//   T3LayerGroupNode

SO_NODE_SOURCE(T3LayerGroupNode);

void T3LayerGroupNode::initClass()
{
  SO_NODE_INIT_CLASS(T3LayerGroupNode, T3NodeParent, "T3NodeParent");
}

extern void T3LayerGroupNode_XYDragFinishCB(void* userData, SoDragger* dragger);
extern void T3LayerGroupNode_ZDragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

T3LayerGroupNode::T3LayerGroupNode(void* dataView_, bool show_draggers, bool hide_lines)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3LayerGroupNode);

  show_drag_ = show_draggers;
  hide_lines_ = hide_lines;

  if(show_drag_) {
    const float len = .08f;	// bar_len
    const float wd = .1f * len;	// bar_width
    const float cr = .2f * len;	// cone radius
    const float ch = .4f * len;	// cone height

    // XY dragger
    xy_drag_sep_ = new SoSeparator;
    xy_drag_xf_ = new SoTransform;
    xy_drag_xf_->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), -1.5707963f);
    xy_drag_sep_->addChild(xy_drag_xf_);
    xy_dragger_ = new SoTranslate2Dragger;
    xy_dragger_->setPart("translator", new T3Translate2Translator(false, len, wd, cr, ch));
    xy_dragger_->setPart("translatorActive", new T3Translate2Translator(true, len, wd, cr, ch));
    xy_drag_sep_->addChild(xy_dragger_);

    topSeparator()->addChild(xy_drag_sep_);

    // A = XY
    xy_drag_calc_ = new SoCalculator;
    xy_drag_calc_->ref();
    xy_drag_calc_->A.connectFrom(&xy_dragger_->translation);

    xy_dragger_->addFinishCallback(T3LayerGroupNode_XYDragFinishCB, (void*)this);

    // Z dragger
    z_drag_sep_ = new SoSeparator;
    z_drag_xf_ = new SoTransform;
    z_drag_xf_->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5707963f);
    z_drag_sep_->addChild(z_drag_xf_);
    z_dragger_ = new SoTranslate1Dragger;
    z_dragger_->setPart("translator", new T3Translate1Translator(false, len, wd, cr, ch));
    z_dragger_->setPart("translatorActive", new T3Translate1Translator(true, len, wd, cr, ch));
    z_drag_sep_->addChild(z_dragger_);

    topSeparator()->addChild(z_drag_sep_);

    // B = Z
    xy_drag_calc_->B.connectFrom(&z_dragger_->translation);
    //    xy_drag_calc_->expression = "oA = vec3f(.5 + A[0], B[0], -(.5 + A[1]))";
    txfm_shape()->translation.connectFrom(&xy_drag_calc_->oA);

    z_dragger_->addFinishCallback(T3LayerGroupNode_ZDragFinishCB, (void*)this);
  }
  else {
    xy_drag_sep_ = NULL;
    xy_drag_xf_ = NULL;
    xy_dragger_ = NULL;
    xy_drag_calc_ = NULL;

    z_drag_sep_ = NULL;
    z_drag_xf_ = NULL;
    z_dragger_ = NULL;
    z_drag_calc_ = NULL;
  }

  if(!hide_lines_) {
    SoSeparator* ss = shapeSeparator(); // cache
    drw_styl_ = new SoDrawStyle;
    drw_styl_->style = SoDrawStyle::LINES;
    ss->addChild(drw_styl_);
    SoMaterial* mat = material();
    mat->diffuseColor.setValue(0.8f, 0.5f, 0.8f); // lighter violet than draggers
    mat->transparency.setValue(0.3f);		// less transparent
    shape_ = new SoLineBox3d;
    ss->addChild(shape_);
  }
  else {
    drw_styl_ = NULL;		// no shape
    shape_ = NULL;		// no shape
  }
}

T3LayerGroupNode::~T3LayerGroupNode()
{
//  shape_ = NULL;
}

void T3LayerGroupNode::render() {
  if(!shape_) return;

  float fx = ((float)lgp_max_size.x + 2.0f * T3LayerNode::width) / max_size.x;
  float fy = ((float)lgp_max_size.y + 2.0f * T3LayerNode::width) / max_size.y;
  float fz = ((float)(lgp_max_size.z-1) + 4.0f * T3LayerNode::height) / max_size.z;
  float lay_wd_x = (T3LayerNode::width / max_size.x);
  float lay_wd_y = (T3LayerNode::width / max_size.y);
  lay_wd_x = MIN(lay_wd_x, T3LayerNode::max_width);
  lay_wd_y = MIN(lay_wd_y, T3LayerNode::max_width);
  float lay_ht_z = 2.0f * (T3LayerNode::height / max_size.z);
  float xfrac = (.5f * fx) - lay_wd_x;
  float yfrac = (.5f * fy) - lay_wd_y;
  float zfrac = (.5f * fz) - lay_ht_z;

  shape_->width = fx;
  shape_->height = fz;
  shape_->depth = fy;
  shape_->render();

  txfm_shape()->translation.setValue(xfrac, zfrac, -yfrac); // move to 0,0

  if(show_drag_) {
    float len = .08f;	// bar_len
    len = MIN(len, .5f * fx);

    float wd = .1f * len;	// bar_width
    float cr = .2f * len;	// cone radius
    float ch = .4f * len;	// cone height

    String expr = "oA = vec3f(" + String(xfrac) + " + A[0], "
      + String(zfrac) + " + B[0], -(" + String(yfrac) + " + A[1]))";
    xy_drag_calc_->expression = expr.chars();

    xy_drag_xf_->translation.setValue(-lay_wd_x, -lay_ht_z, lay_wd_y);
    z_drag_xf_->translation.setValue(-lay_wd_x, -lay_ht_z, lay_wd_y);

    if(len != .08f) {
      xy_dragger_->setPart("translator", new T3Translate2Translator(false, len, wd, cr, ch));
      xy_dragger_->setPart("translatorActive", new T3Translate2Translator(true, len, wd, cr, ch));
      z_dragger_->setPart("translator", new T3Translate1Translator(false, len, wd, cr, ch));
      z_dragger_->setPart("translatorActive", new T3Translate1Translator(true, len, wd, cr, ch));
    }
  }
}

void T3LayerGroupNode::setGeom(int px, int py, int pz,
			       float lg_max_x, float lg_max_y, float lg_max_z,
			       float max_x, float max_y, float max_z) {
  pos.setValue(px, py, pz);
  max_size.setValue(max_x, max_y, max_z);
  lgp_max_size.setValue(lg_max_x, lg_max_y, lg_max_z);
  render();
}


/////////////////////////////////////////////
//	NetViewObj

SO_NODE_SOURCE(T3NetViewObj);

void T3NetViewObj::initClass()
{
  SO_NODE_INIT_CLASS(T3NetViewObj, T3NodeLeaf, "T3NodeLeaf");
}

extern void T3NetViewObj_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

T3NetViewObj::T3NetViewObj(void* obj, bool show_drag)
:inherited(obj)
{
  SO_NODE_CONSTRUCTOR(T3NetViewObj);

  show_drag_ = show_drag;
  drag_ = NULL;
  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);

    txfm_shape()->translation.connectFrom(&drag_->dragger_->translation);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3NetViewObj_DragFinishCB, (void*)this);
    insertChildBefore(topSeparator(), drag_, txfm_shape());
  }
}

T3NetViewObj::~T3NetViewObj()
{
  
}

//////////////////////////
//   T3NetNode		//
//////////////////////////

extern void T3NetNode_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso
extern void T3NetText_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

SO_NODE_SOURCE(T3NetNode);

void T3NetNode::initClass()
{
  SO_NODE_INIT_CLASS(T3NetNode, T3NodeParent, "T3NodeParent");
}

T3NetNode::T3NetNode(void* dataView_, bool show_draggers,
		     bool show_net_text, bool show_nt_drag)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3NetNode);

  show_drag_ = show_draggers;
  show_net_text_drag_ = show_nt_drag && show_net_text;

  float net_margin = 0.05f;
  float two_net_marg = 2.0f * net_margin;
  txfm_shape()->translation.setValue(-net_margin, -net_margin, -net_margin);

  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);

    drag_->xf_->translation.setValue(-net_margin, -net_margin, -net_margin);
    String expr = "oA = vec3f(-" + String(net_margin) + " + A[0], -"
      + String(net_margin) + " + A[1], -" + String(net_margin) + " + A[2])";
    drag_->trans_calc_->expression = expr.chars();

    txfm_shape()->translation.connectFrom(&drag_->trans_calc_->oA);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3NetNode_DragFinishCB, (void*)this);
    topSeparator()->addChild(drag_);
    SoSeparator* ss = shapeSeparator(); // cache
    shape_draw_ = new SoDrawStyle;
    shape_draw_->style = SoDrawStyle::LINES;
    ss->addChild(shape_draw_);
    shape_ = new SoLineBox3d(1.0f + two_net_marg, 1.0f + two_net_marg, 1.0f + two_net_marg, false); // false = no center
    ss->addChild(shape_);
  }
  else {
    shape_ = NULL;
    shape_draw_ = NULL;
  }

  net_text_ = NULL;
  net_text_drag_ = NULL;
  net_text_xform_ = NULL;
  net_text_drag_xform_ = NULL;
  if(show_net_text) {
    net_text_ = new SoSeparator;
    addChild(net_text_);
    net_text_xform_ = new SoTransform;
    net_text_xform_->translation.setValue(0.0f, 0.0f, 0.0f);
    net_text_->addChild(net_text_xform_);
    if(show_net_text_drag_) {
      net_text_drag_xform_ = new SoTransform;
      net_text_drag_ = new T3TransformBoxDragger(0.04f, .03f, .02f);

      net_text_drag_->xf_->translation.setValue(1.0f, 0.0f, 0.0f);
      String expr = "oA = vec3f(1.0 + A[0], A[1], A[2])";
      net_text_drag_->trans_calc_->expression = expr.chars();

      net_text_drag_xform_->translation.connectFrom(&net_text_drag_->dragger_->translation);
      net_text_drag_xform_->rotation.connectFrom(&net_text_drag_->dragger_->rotation);
      net_text_drag_xform_->scaleFactor.connectFrom(&net_text_drag_->dragger_->scaleFactor);

      net_text_drag_->dragger_->addFinishCallback(T3NetText_DragFinishCB, (void*)this);
      net_text_->addChild(net_text_drag_); // dragger then the xform it affects
      net_text_->addChild(net_text_drag_xform_);
      // we then copy the drag_xform back into the main xform in the CB, to effect the change
    }
  }
  wt_lines_ = new SoSeparator;
  wt_lines_->setName("WtLines");
  wt_lines_draw_ = new SoDrawStyle;
  wt_lines_->addChild(wt_lines_draw_);
  wt_lines_set_ = new SoIndexedLineSet;
  wt_lines_vtx_prop_ = new SoVertexProperty;
  wt_lines_set_->vertexProperty.setValue(wt_lines_vtx_prop_); // does ref/unref
  wt_lines_->addChild(wt_lines_set_);
  addChild(wt_lines_);
}

T3NetNode::~T3NetNode()
{
}

void T3NetNode::setDefaultCaptionTransform() {
  SbVec3f tran(0.0f, -.1f, 0.03f);
  transformCaption(tran);
}

