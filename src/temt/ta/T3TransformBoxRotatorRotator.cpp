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

#include "T3TransformBoxRotatorRotator.h"

SO_NODE_SOURCE(T3TransformBoxRotatorRotator);

void T3TransformBoxRotatorRotator::initClass()
{
  SO_NODE_INIT_CLASS(T3TransformBoxRotatorRotator, SoSeparator, "SoSeparator");
}

T3TransformBoxRotatorRotator::T3TransformBoxRotatorRotator(bool active, float half_size, float line_width) {
  SO_NODE_CONSTRUCTOR(T3TransformBoxRotatorRotator);

  SoMaterial* mat = new SoMaterial;
  if(active) {
    mat->diffuseColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->emissiveColor.setValue(drag_activ_clr_r, drag_activ_clr_g, drag_activ_clr_b);
    mat->transparency.setValue(drag_activ_clr_tr);
  }
  else {
    mat->diffuseColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->emissiveColor.setValue(drag_inact_clr_r, drag_inact_clr_g, drag_inact_clr_b);
    mat->transparency.setValue(drag_inact_clr_tr);
  }
  addChild(mat);

  float sz = half_size;
  float sz2 = 2.0f * sz;

  SoCube* cb = new SoCube;
  cb->width = line_width;
  cb->height = sz2;
  cb->depth = line_width;

  SoGroup* pts = new SoGroup;
  addChild(pts);
  float x,z;
  for(z=-sz;z<=sz;z+=sz2) {
    for(x=-sz;x<=sz;x+=sz2) {
      SoSeparator* sep = new SoSeparator;
      pts->addChild(sep);
      SoTranslation* trn = new SoTranslation;
      trn->translation.setValue(x,0.0f,z);
      sep->addChild(trn);
      sep->addChild(cb);
    }
  }
}
