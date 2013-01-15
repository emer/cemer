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

SO_NODE_SOURCE(T3VEWorld);

void T3VEWorld::initClass()
{
  SO_NODE_INIT_CLASS(T3VEWorld, T3NodeParent, "T3NodeParent");
}

T3VEWorld::T3VEWorld(T3DataView* world)
  : inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VEWorld);

  sun_light = new SoDirectionalLight;
  insertChildBefore(topSeparator(), sun_light, childNodes());
  sun_light->on = false;

  cam_light = new SoDirectionalLight;
  insertChildBefore(topSeparator(), cam_light, childNodes());
  cam_light->on = false;

  light_group = new SoGroup;
  insertChildBefore(topSeparator(), light_group, childNodes());

  camera_switch = new SoSwitch;
  camera_switch->whichChild = -1; // no cameras!
  insertChildBefore(topSeparator(), camera_switch, childNodes());

  textures = new SoSwitch;
  textures->whichChild = -1;	// don't render here!
  insertChildBefore(topSeparator(), textures, childNodes());

  texture_xforms = new SoSwitch;
  texture_xforms->whichChild = -1;	// don't render here!
  insertChildBefore(topSeparator(), texture_xforms, childNodes());
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

