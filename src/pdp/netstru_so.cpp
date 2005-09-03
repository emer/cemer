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

#include <math.h>
#include <limits.h>


// netview stuff
const float NET_VIEW_INITIAL_X = 330.0f;
const float NET_VIEW_INITIAL_Y = 300.0f;
const float NET_VIEW_X_MARG = 8.0f;
const float NET_VIEW_Y_MARG = 16.0f;

// projection_g stuff
const float SELF_CON_DIST = .03f;

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))


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

T3UnitNode::T3UnitNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode);
}

T3UnitNode::~T3UnitNode()
{
}

void T3UnitNode::setAppearance(float act, const T3Color& color) {
  material()->diffuseColor = (SbColor)color;
}

void T3UnitNode::setDefaultCaptionTransform() {
  //note: this is the one for 3d objects -- 2d replace this
  captionNode_->justification = SoAsciiText::CENTER;
  transformCaption(SbVec3f(0.0f, 0.1f, 0.45f));
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

float T3UnitNode_Cylinder::shape_radius = 0.4f; //  (also used for _Circle)

SO_NODE_SOURCE(T3UnitNode_Cylinder);

void T3UnitNode_Cylinder::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Cylinder, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Cylinder::T3UnitNode_Cylinder(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Cylinder);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = shape_radius; // always the same radius
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f)); //dk gray
}

T3UnitNode_Cylinder::~T3UnitNode_Cylinder()
{
  shape_ = NULL;
}

void T3UnitNode_Cylinder::setAppearance(float act, const T3Color& color) {
  if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  shape_->height = base_height + ((max_height - base_height) * fabs(act));
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = ((max_height - base_height) * act) * 0.5f;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance(act, color);
}

SO_NODE_SOURCE(T3UnitNode_Circle);

void T3UnitNode_Circle::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Circle, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Circle::T3UnitNode_Circle(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Circle);

  shape_ = new SoCylinder; // note: same shape is used for both styles
  shape_->radius = T3UnitNode_Cylinder::shape_radius; // always the same radius
  shape_->height = 0.01f;
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f)); //dk gray
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

float T3UnitNode_Block::shape_width = 0.8f; //  (also used for _Rect)
float T3UnitNode_Block::shape_depth = 0.6f; //  (also used for _Rect)

SO_NODE_SOURCE(T3UnitNode_Block);

void T3UnitNode_Block::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Block, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Block::T3UnitNode_Block(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Block);

  shape_ = new SoCube; // note: same shape is used for both styles
  shape_->width = shape_width; // always the same radius
  shape_->depth = shape_depth; // always the same radius
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f)); //dk gray
}

T3UnitNode_Block::~T3UnitNode_Block()
{
  shape_ = NULL;
}

void T3UnitNode_Block::setAppearance(float act, const T3Color& color) {
  if (act < -1.0f) act = -1.0f;
  else if (act > 1.0f) act = 1.0f;
  shape_->height = base_height + ((max_height - base_height) * fabs(act));
  float dz; float x; float y;
  transform()->translation.getValue().getValue(x, dz, y);
  dz = ((max_height - base_height) * act) * 0.5f;
  transform()->translation.setValue(x, dz, y);
  inherited::setAppearance(act, color);
}

//////////////////////////
//   T3UnitNode_Rect//
//////////////////////////

SO_NODE_SOURCE(T3UnitNode_Rect);

void T3UnitNode_Rect::initClass()
{
  SO_NODE_INIT_CLASS(T3UnitNode_Rect, T3UnitNode, "T3UnitNode");
}

T3UnitNode_Rect::T3UnitNode_Rect(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitNode_Rect);

  shape_ = new SoCube; // note: same shape is used for both styles
  shape_->width = T3UnitNode_Block::shape_width; // always the same radius
  shape_->depth = T3UnitNode_Block::shape_depth; // always the same radius
  shape_->height = 0.01f;
  shapeSeparator()->addChild(shape_);

  // bake in the shape offset right into our origin offset, to avoid an additional txfm
//  SetOrigin(pos.x + 0.5f, pos.y + 0.5f, h/2.0f);
  setAppearance(0.0f, T3Color(.25f, .25f, .25f)); //dk gray
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
    drawGrid(node);
  }
}

void T3UnitGroupNode::drawGrid(T3UnitGroupNode* node) {
  float sw = 0.02f; // strip width
  float x_end = (float)node->geom.x;
  float y_end = (float)(-node->geom.y);
  GLbitfield attribs = (GLbitfield)(GL_LIGHTING_BIT | GL_TRANSFORM_BIT);
  glPushMatrix();
  glPushAttrib(attribs); //note: doesn't seem to push matrix properly
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  glDisable(GL_LIGHTING);
  glColor3f(0.4f, 0.4f, 0.4f);
  // vert lines
  for (int x = 1; x < node->geom.x; ++x) {
    glRectf(x - sw, 0.0f, x + sw, y_end);
  }
  // hor lines
  for (int y = 1; y < node->geom.y; ++y) {
    glRectf(0.0f, -(y - sw), x_end, -(y + sw));
  }
  glPopAttrib();
  glPopMatrix();
}

T3UnitGroupNode::T3UnitGroupNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3UnitGroupNode);
  unitCaptionFont_ = NULL;
//  SoSeparator* ss = shapeSeparator();

  SoCallback* cb = new SoCallback();
  cb->setCallback(shapeCallback, (void*)this);
  insertChildAfter(topSeparator(), cb, transform());

/*  shape_ = new SoCube;
  shape_->setName("shape");
  ss->addChild(shape_); */

/*obs  units_ = new SoGroup;
  topSeparator()->addChild(units_); */
}

T3UnitGroupNode::~T3UnitGroupNode()
{
  shape_ = NULL;
//obs  units_ = NULL;
  SoMaterial* mat = material();
  mat->diffuseColor.setValue(1.0f, 1.0f, 1.0f); // white (invisible)
  mat->transparency.setValue(1.0f);
}

void T3UnitGroupNode::setGeom(int x, int y) {
  if (geom.isEqual(x, y)) return; // nothing to do, not changed

/*  float h = 0.0f; // nominal amount of height, so we don't vanish
  // set size/pos of cube -- note we are vertically centered in layer
  txfm_shape()->translation.setValue(x/2.0f, 0.0f, -y/2.0f);
  SoCube* shp = shape();
  shp->width = x - (2 * inset);
  shp->height = h;
  shp->depth = y -  (2 * inset);
*/
/*obs  int numUnits = x * y;
  // make sure correct number of units are created for the geom
  if (units_->getNumChildren() > numUnits) { // truncate
    for (int i = units_->getNumChildren() - 1; i >= numUnits; --i) {
      units_->removeChild(i);
    }
  } else { // expand or do nothing
    for (int i = units_->getNumChildren(); i < numUnits; ++i) {
      T3UnitNode* un = new T3UnitNode(this);
      units_->addChild(un);
    }
  }
  // set the appropriate origin for each unit
  int idx = 0;
  for (int yi = 0; yi < y; ++yi) {
    for (int xi = 0; xi < x; ++xi) {
      T3UnitNode* un = (T3UnitNode*)units_->getChild(idx);
      un->setPos(xi, yi);
      ++idx;
    }
  } */

  geom.setValue(x, y);
}

SoFont* T3UnitGroupNode::unitCaptionFont(bool auto_create) {
  if (unitCaptionFont_ || !auto_create) return unitCaptionFont_;
  if (!unitCaptionFont_) {
    unitCaptionFont_ = new SoFont();
    insertChildBefore(topSeparator(), unitCaptionFont_, childNodes());
  }
  return unitCaptionFont_;
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
  material()->diffuseColor.setValue(0.4f, 0.4f, 0.4f);

  SoSeparator* ss = shapeSeparator(); // cache

  shape_ = new SoFrame();
  ss->addChild(shape_);
}

T3LayerNode::~T3LayerNode()
{
//  shape_ = NULL;
}

void T3LayerNode::render() {
//  float h = height;
  int x; int y;
  geom.getValue(x, y);
  shape_->setDimensions(x, y, 0.3f, 0.2f);
  // note: LayerView already translates us up into vertical center of cell
  txfm_shape()->translation.setValue(x/2.0f, 0.0f, -y/2.0f);
}

void T3LayerNode::setGeom(int x, int y) {
  if (geom.isEqual(x, y)) return; // nothing to do, not changed
  geom.setValue(x, y);
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

T3PrjnNode::T3PrjnNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3PrjnNode);
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

  //color and material
  material()->diffuseColor.setValue(SbColor(.5f, .5f, .5f)); // medium gray

  float rad = 0.01f;
  line_prjn = new SoCylinder();
  line_prjn->radius = rad; //height is variable, depends on length
  ss->addChild(line_prjn);

  trln_arr = new SoTransform();
  ss->addChild(trln_arr);

  arr_prjn = new SoCone();
  arr_prjn->height =  3.0f * rad;
  arr_prjn->bottomRadius = 2.0f * rad;
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

  shape_ = new SoCube;
  shape_->setName("shape");
  shapeSeparator()->addChild(shape_);

  float h = 0.04f; // nominal amount of height, so we don't vanish
  float x = 1.0f;
  float y = 1.0f;
  // set size/pos of cube -- move down to -1 y
  txfm_shape()->translation.setValue(x/2.0f, h/2.0f - 1.0f, -y/2.0f);
  shape_->width = x;
  shape_->height = h;
  shape_->depth = y;

}

T3NetNode::~T3NetNode()
{
}

void T3NetNode::setDefaultCaptionTransform() {
  SbVec3f tran(0.0f, -1.0f, 0.0f);
  transformCaption(tran);
}


//////////////////////////
//   T3LogViewNode	//
//////////////////////////

SO_NODE_SOURCE(T3LogViewNode);

void T3LogViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3LogViewNode, T3NodeParent, "T3NodeParent");
}

T3LogViewNode::T3LogViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3LogViewNode);

  frame_ = NULL;
  canvasSeparator_ = new SoSeparator();
  txfm_canvas_ = new SoTransform();
  canvasSeparator_->addChild(txfm_canvas_);
  canvas_ = new SoSeparator();
  canvasSeparator_->addChild(canvas_);
  insertChildAfter(topSeparator(), canvasSeparator_, shapeSeparator());
  geom_.setValue(1, 1, 1); // puts in valid state
}

T3LogViewNode::~T3LogViewNode()
{
  canvas_ = NULL;
  txfm_canvas_ = NULL;
  canvasSeparator_ = NULL;
  frame_ = NULL;
}

void T3LogViewNode::render() {
  float inset = (frame_) ? frame_->inset : 0.0f;
  if (frame_) {
    frame_->setDimensions(geom_.x, geom_.z, 0.1f, 0.05f);
    // compensate canvas area for inset of frame
    txfm_canvas_->scaleFactor.setValue(
      (geom_.x - (2 * inset)) / geom_.x,
      (geom_.z - (2 * inset)) / geom_.z,
      1.0f);
  } else {
    txfm_canvas_->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  }
  txfm_shape()->translation.setValue(geom_.x/2.0f, geom_.z/2.0f, 0.0f);
  txfm_canvas_->translation.setValue(inset, inset, 0.0f);
}

void T3LogViewNode::setGeom(int px, int py, int pz) {
  if (px < 1) px = 1;  if (pz < 1) py = 1;  if (pz < 1) pz = 1;
  if (geom_.isEqual(px, py, pz)) return;
  geom_.setValue(px, py, pz);
  render();
}

void T3LogViewNode::setShowFrame(bool value) {
  if (showFrame() == value) return;
  SoSeparator* ss = shapeSeparator(); // cache
  if (value) {
    frame_ = new SoFrame(SoFrame::Ver);
    //frame_ = new SoFrame(SoFrame::Hor);
    insertChildAfter(ss, frame_, material());
  } else {
    ss->removeChild(frame_);
    frame_ = NULL;
  }
  render();
}

bool T3LogViewNode::showFrame() {
  return frame_;
}


//////////////////////////
//   T3GridLogViewBaseNode	//
//////////////////////////

SO_NODE_SOURCE(T3GridLogViewBaseNode);

void T3GridLogViewBaseNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GridLogViewBaseNode, T3LogViewNode, "T3LogViewNode");
}

T3GridLogViewBaseNode::T3GridLogViewBaseNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GridLogViewBaseNode);
  header_ = new SoGroup();
  canvas_->addChild(header_);
  body_ = new SoGroup();
  canvas_->addChild(body_);
}

T3GridLogViewBaseNode::~T3GridLogViewBaseNode()
{
  header_ = NULL;
  body_ = NULL;
}


//////////////////////////
//   T3GraphLogViewNode	//
//////////////////////////

SO_NODE_SOURCE(T3GraphLogViewNode);

void T3GraphLogViewNode::initClass()
{
  SO_NODE_INIT_CLASS(T3GraphLogViewNode, T3LogViewNode, "T3LogViewNode");
}

T3GraphLogViewNode::T3GraphLogViewNode(void* dataView_)
:inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3GraphLogViewNode);
/*  header_ = new SoGroup();
  canvas_->addChild(header_);
  body_ = new SoGroup();
  canvas_->addChild(body_); */
}

T3GraphLogViewNode::~T3GraphLogViewNode()
{
/*  header_ = NULL;
  body_ = NULL; */
}


