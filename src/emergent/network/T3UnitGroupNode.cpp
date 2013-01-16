// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "T3UnitGroupNode.h"

#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

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

T3UnitGroupNode::T3UnitGroupNode(T3DataView* dataView_, bool no_unts)
  : inherited(dataView_)
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
//   SoMaterial* mat = material();
//   mat->diffuseColor.setValue(1.0f, 1.0f, 1.0f); // white (invisible)
//   mat->transparency.setValue(1.0f);
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

