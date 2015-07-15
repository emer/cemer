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

#include "T3Translate1Translator.h"
#include <T3Misc>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoTransform.h>


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

