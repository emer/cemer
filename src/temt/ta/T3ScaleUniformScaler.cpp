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

#include "T3ScaleUniformScaler.h"
#include <T3Misc>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>


SO_NODE_SOURCE(T3ScaleUniformScaler);

void T3ScaleUniformScaler::initClass()
{
  SO_NODE_INIT_CLASS(T3ScaleUniformScaler, SoSeparator, "SoSeparator");
}

T3ScaleUniformScaler::T3ScaleUniformScaler(bool active, float half_size, float cube_size) {
  SO_NODE_CONSTRUCTOR(T3ScaleUniformScaler);

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

  float sz = half_size;
  float sz2 = 2.0f * half_size;

  SoCube* cb = new SoCube;
  cb->width = cube_size;
  cb->height = cube_size;
  cb->depth = cube_size;

  SoGroup* pts = new SoGroup;
  addChild(pts);
  float x,y,z;
  for(z=-sz;z<=sz;z+=sz2) {
    for(y=-sz;y<=sz;y+=sz2) {
      for(x=-sz;x<=sz;x+=sz2) {
	SoSeparator* sep = new SoSeparator;
	pts->addChild(sep);
	SoTranslation* trn = new SoTranslation;
	trn->translation.setValue(x,y,z);
	sep->addChild(trn);
	sep->addChild(cb);
      }
    }
  }
}

