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

#include "T3Translate2Translator.h"
#include <T3Misc>
#include <taMath_float>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoTransform.h>


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
    mat->diffuseColor.setValue(T3Misc::drag_activ_clr_r, T3Misc::drag_activ_clr_g, T3Misc::drag_activ_clr_b);
    mat->emissiveColor.setValue(T3Misc::drag_activ_clr_r, T3Misc::drag_activ_clr_g, T3Misc::drag_activ_clr_b);
    mat->transparency.setValue(T3Misc::drag_activ_clr_tr);
  }
  else {
    mat->diffuseColor.setValue(T3Misc::drag_inact_clr_r, T3Misc::drag_inact_clr_g, T3Misc::drag_inact_clr_b);
    mat->emissiveColor.setValue(T3Misc::drag_inact_clr_r, T3Misc::drag_inact_clr_g, T3Misc::drag_inact_clr_b);
    mat->transparency.setValue(T3Misc::drag_inact_clr_tr);
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
  yc2t->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), taMath_float::pi);
  yc2s->addChild(yc2t);
  yc2s->addChild(cc);
  addChild(yc2s);
}

