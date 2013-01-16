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

#include "T3LayerGroupNode.h"

#include <SoLineBox3d>
#include <T3Translate1Translator>
#include <T3Translate2Translator>

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/engines/SoCalculator.h>


SO_NODE_SOURCE(T3LayerGroupNode);

void T3LayerGroupNode::initClass()
{
  SO_NODE_INIT_CLASS(T3LayerGroupNode, T3NodeParent, "T3NodeParent");
}

extern void T3LayerGroupNode_XYDragFinishCB(void* userData, SoDragger* dragger);
extern void T3LayerGroupNode_ZDragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

T3LayerGroupNode::T3LayerGroupNode(T3DataView* dataView_, bool show_draggers,
				   bool hide_lines, bool md_2d)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3LayerGroupNode);

  show_drag_ = show_draggers;
  hide_lines_ = hide_lines;
  mode_2d_ = md_2d;

  if(show_drag_) {
    const float len = .05f;	// bar_len
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

      z_dragger_->addFinishCallback(T3LayerGroupNode_ZDragFinishCB, (void*)this);
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

    if(mode_2d_) {
      xy_drag_xf_->translation.setValue(-lay_wd_x, -lay_ht_z, lay_wd_y);
    }
    else {
      xy_drag_xf_->translation.setValue(-lay_wd_x, lay_ht_z, lay_wd_y);
      z_drag_xf_->translation.setValue(-lay_wd_x, lay_ht_z, lay_wd_y);
    }

    if(len != .08f) {
      xy_dragger_->setPart("translator", new T3Translate2Translator(false, len, wd, cr, ch));
      xy_dragger_->setPart("translatorActive", new T3Translate2Translator(true, len, wd, cr, ch));

      if(z_dragger_) {
	z_dragger_->setPart("translator", new T3Translate1Translator(false, len, wd,
								     cr, ch));
	z_dragger_->setPart("translatorActive", new T3Translate1Translator(true, len,
									   wd, cr, ch));
      }
    }
  }
}

void T3LayerGroupNode::setGeom(int px, int py, int pz,
			       float lg_max_x, float lg_max_y, float lg_max_z,
			       float max_x, float max_y, float max_z) {
  pos.setValue(px, py, pz);
  max_size.setValue(max_x, max_y, max_z);
  lgp_max_size.setValue((int)lg_max_x, (int)lg_max_y, (int)lg_max_z);
  render();
}

