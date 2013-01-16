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

#include "T3LayerNode.h"
#include <T3Translate1Translator>
#include <T3Translate2Translator>


#include <Inventor/nodes/SoTransform.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>


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

T3LayerNode::T3LayerNode(T3DataView* dataView_, bool show_draggers, bool md_2d)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3LayerNode);

  show_drag_ = show_draggers;
  mode_2d_ = md_2d;

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
    if(mode_2d_) {
      z_drag_sep_ = NULL;
      z_drag_xf_ = NULL;
      z_dragger_ = NULL;
      z_drag_calc_ = NULL;
    }
    else {
      z_drag_sep_ = new SoSeparator;
      z_drag_xf_ = new SoTransform;
      z_drag_xf_->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5707963f);
      z_drag_sep_->addChild(z_drag_xf_);
      z_dragger_ = new SoTranslate1Dragger;
      z_dragger_->setPart("translator", new T3Translate1Translator(false, len, wd, cr, ch));
      z_dragger_->setPart("translatorActive", new T3Translate1Translator(true, len, wd, cr, ch));
      z_drag_sep_->addChild(z_dragger_);

      topSeparator()->addChild(z_drag_sep_);

      z_dragger_->addFinishCallback(T3LayerNode_ZDragFinishCB, (void*)this);
      // B = Z
      xy_drag_calc_->B.connectFrom(&z_dragger_->translation);
    }
    //    xy_drag_calc_->expression = "oA = vec3f(.5 + A[0], B[0], -(.5 + A[1]))";
    txfm_shape()->translation.connectFrom(&xy_drag_calc_->oA);
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
    float len = .05f;	// bar_len
    len = MIN(len, .5f * fx);
    len = MAX(.02f, len);	// keep it above a small min

    float wd = .1f * len;	// bar_width
    float cr = .2f * len;	// cone radius
    float ch = .4f * len;	// cone height

    String expr = "oA = vec3f(" + String(xfrac) + " + A[0], B[0], -(" + String(yfrac) + " + A[1]))";
    xy_drag_calc_->expression = expr.chars();

    if(len != .08f) {
      xy_dragger_->setPart("translator", new T3Translate2Translator(false, len, wd,
								    cr, ch));
      xy_dragger_->setPart("translatorActive", new T3Translate2Translator(true, len,
									  wd, cr, ch));
      if(z_dragger_) {
	z_dragger_->setPart("translator", new T3Translate1Translator(false, len, wd,
								     cr, ch));
	z_dragger_->setPart("translatorActive", new T3Translate1Translator(true, len,
									   wd, cr, ch));
      }
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

