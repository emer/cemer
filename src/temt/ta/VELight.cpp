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

#include "VELight.h"

#include <VEWorld>
#include <VEWorldView>
#include <T3VEWorld>
#include <taMath_float>

TA_BASEFUNS_CTORS_DEFN(VELight);
SMARTREF_OF_CPP(VELight); // VELightRef

#ifdef TA_QT3D

#else // TA_QT3D
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoSpotLight.h>

#endif // TA_QT3D

void VELight::Initialize() {
  flags = (BodyFlags)(FIXED | NO_COLLIDE | EULER_ROT);
  light_type = DIRECTIONAL_LIGHT;
  drop_off_rate = 0.1f;
  cut_off_angle = 30.0f;
  dir_norm.x = 0.0f; dir_norm.y = 0.0f; dir_norm.z = -1.0f;
  shape = CYLINDER;  // having a shape will interfere with spotlight!  but might want to see where it is sometimes..
  long_axis = LONG_Z;
  mass = .1f;
  radius = .1f;
  length = .2f;
}

void VELight::UpdtDirNorm() {
  taVector3f dn(0.0f, 0.0f, -1.0f);
  cur_quat.RotateVec(dn);
  dir_norm = dn;
}

void VELight::Init() {
  inherited::Init();
  UpdtDirNorm();
}

void VELight::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);
  UpdtDirNorm();
}

void VELight::CurToODE() {
  inherited::CurToODE();
  UpdtDirNorm();
}

#ifdef TA_QT3D


bool VELight::UpdateLight() {
  UpdtDirNorm();
  bool rval = false;
  VEWorld* wrld = GET_MY_OWNER(VEWorld);
  if(!wrld) return false;
  VEWorldView* wv = wrld->FindView();
  if(!wv || !wv->node_so()) return false;
  // SoGroup* lg = ((T3VEWorld*)wv->node_so())->getLightGroup();
  // if(!lg || lg->getNumChildren() == 0) return false;
  // if(wrld->light_0.ptr() == this) {
  //   SoLight* lgt = (SoLight*)lg->getChild(0);
  //   ConfigLight(lgt);
  //   rval = true;
  // }
  // else if(wrld->light_1.ptr() == this) {
  //   if(lg->getNumChildren() == 2) {
  //     SoLight* lgt = (SoLight*)lg->getChild(1);
  //     ConfigLight(lgt);
  //   }
  //   else {
  //     SoLight* lgt = (SoLight*)lg->getChild(0);
  //     ConfigLight(lgt);
  //   }
  //   rval = true;
  // }
  return rval;
}

#else // TA_QT3D

SoLight* VELight::CreateLight() {
  SoLight* lgt = NULL;
  switch(light_type) {
  case DIRECTIONAL_LIGHT:
    lgt = new SoDirectionalLight;
    break;
  case POINT_LIGHT:
    lgt = new SoPointLight;
    break;
  case SPOT_LIGHT:
    lgt = new SoSpotLight;
    break;
  }
  ConfigLight(lgt);
  return lgt;
}

void VELight::ConfigLight(SoLight* lgt) {
  lgt->on = light.on;
  lgt->intensity = light.intensity;
  lgt->color.setValue(light.color.r, light.color.g, light.color.b);
  switch(light_type) {
  case DIRECTIONAL_LIGHT:
    ((SoDirectionalLight*)lgt)->direction.setValue(dir_norm.x, dir_norm.y, dir_norm.z);
    break;
  case POINT_LIGHT:
    ((SoPointLight*)lgt)->location.setValue(cur_pos.x, cur_pos.y, cur_pos.z);
    break;
  case SPOT_LIGHT:
    SoSpotLight* sl = (SoSpotLight*)lgt;
    sl->direction.setValue(dir_norm.x, dir_norm.y, dir_norm.z);
    float hl = .5f * length;
    sl->location.setValue(cur_pos.x + dir_norm.x * hl,
                          cur_pos.y + dir_norm.y * hl,
                          cur_pos.z + dir_norm.z * hl);
    sl->dropOffRate = drop_off_rate;
    sl->cutOffAngle = cut_off_angle * taMath_float::rad_per_deg;
    break;
  }
}

bool VELight::UpdateLight() {
  UpdtDirNorm();
  bool rval = false;
  VEWorld* wrld = GET_MY_OWNER(VEWorld);
  if(!wrld) return false;
  VEWorldView* wv = wrld->FindView();
  if(!wv || !wv->node_so()) return false;
  SoGroup* lg = ((T3VEWorld*)wv->node_so())->getLightGroup();
  if(!lg || lg->getNumChildren() == 0) return false;
  if(wrld->light_0.ptr() == this) {
    SoLight* lgt = (SoLight*)lg->getChild(0);
    ConfigLight(lgt);
    rval = true;
  }
  else if(wrld->light_1.ptr() == this) {
    if(lg->getNumChildren() == 2) {
      SoLight* lgt = (SoLight*)lg->getChild(1);
      ConfigLight(lgt);
    }
    else {
      SoLight* lgt = (SoLight*)lg->getChild(0);
      ConfigLight(lgt);
    }
    rval = true;
  }
  return rval;
}

#endif // TA_QT3D
