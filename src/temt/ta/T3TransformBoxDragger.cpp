// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "T3TransformBoxDragger.h"
#include <T3Misc>
#include <T3TransformBoxRotatorRotator>
#include <T3Translate2Translator>
#include <T3ScaleUniformScaler>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/engines/SoCalculator.h>


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
