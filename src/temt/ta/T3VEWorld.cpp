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

#include "T3VEWorld.h"

#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/annex/FXViz/nodes/SoShadowGroup.h>
#include <Inventor/annex/FXViz/nodes/SoShadowStyle.h>

SO_NODE_SOURCE(T3VEWorld);

void T3VEWorld::initClass()
{
  SO_NODE_INIT_CLASS(T3VEWorld, T3NodeParent, "T3NodeParent");
}

T3VEWorld::T3VEWorld(T3DataView* world)
  : inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VEWorld);

  camera_switch = new SoSwitch;
  camera_switch->whichChild = -1; // no cameras!
  addChild(camera_switch);

  sun_light = new SoDirectionalLight;
  addChild(sun_light);
  sun_light->on = false;

  shadow_group = new SoShadowGroup;
  addChild(shadow_group);

  cam_light = new SoDirectionalLight;
  shadow_group->addChild(cam_light);
  cam_light->on = false;

  light_group = new SoGroup;
  shadow_group->addChild(light_group);

  textures = new SoSwitch;
  textures->whichChild = -1;	// don't render here!
  shadow_group->addChild(textures);

  texture_xforms = new SoSwitch;
  texture_xforms->whichChild = -1;	// don't render here!
  shadow_group->addChild(texture_xforms);

  shadow_style = new SoShadowStyle;
  shadow_style->style = SoShadowStyle::CASTS_SHADOW_AND_SHADOWED;
  shadow_group->addChild(shadow_style);

  childNodes();                 // build the child nodes

  shadows = false;
}

T3VEWorld::~T3VEWorld()
{
  
}

void T3VEWorld::setSunLightOn(bool on) {
  sun_light->on = on;
}

void T3VEWorld::setSunLightDir(float x_dir, float y_dir, float z_dir) {
  sun_light->direction = SbVec3f(x_dir, y_dir, z_dir);
}

void T3VEWorld::setCamLightOn(bool on) {
  cam_light->on = on;
}

void T3VEWorld::setCamLightDir(float x_dir, float y_dir, float z_dir) {
  cam_light->direction = SbVec3f(x_dir, y_dir, z_dir);
}

void T3VEWorld::setShadows(bool on, float precision, float quality,
                           float smoothing, float near_radius,
                           float far_radius, float threshold, float epsilon) {
  shadow_group->isActive = on;            // just high-level toggle
  shadow_group->precision = precision;
  shadow_group->quality = quality;
  shadow_group->smoothBorder = smoothing;
  shadow_group->visibilityNearRadius = near_radius;
  shadow_group->visibilityRadius = far_radius;
  shadow_group->epsilon = epsilon;
  shadow_group->threshold = threshold;
}

SoSeparator* T3VEWorld::childNodes() {
  if (!childNodes_) {
    childNodes_ = new SoSeparator;
    childNodes_->setName("childNodes");
    shadow_group->addChild(childNodes_);
  }
  return childNodes_;
}

