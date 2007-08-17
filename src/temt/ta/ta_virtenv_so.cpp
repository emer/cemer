// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "ta_virtenv_so.h"

/////////////////////////////////////////////////////////////////////////
//		So Inventor objects

#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoSwitch.h>

SO_NODE_SOURCE(T3VEWorld);

void T3VEWorld::initClass()
{
  SO_NODE_INIT_CLASS(T3VEWorld, T3NodeParent, "T3NodeParent");
}

T3VEWorld::T3VEWorld(void* world)
:inherited(world)
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

/////////////////////////////////////////////
//	Object

SO_NODE_SOURCE(T3VEObject);

void T3VEObject::initClass()
{
  SO_NODE_INIT_CLASS(T3VEObject, T3NodeParent, "T3NodeParent");
}

T3VEObject::T3VEObject(void* world)
:inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VEObject);
}

T3VEObject::~T3VEObject()
{
  
}

/////////////////////////////////////////////
//	Body

SO_NODE_SOURCE(T3VEBody);

void T3VEBody::initClass()
{
  SO_NODE_INIT_CLASS(T3VEBody, T3NodeLeaf, "T3NodeLeaf");
}

T3VEBody::T3VEBody(void* bod, bool show_drag)
:inherited(bod)
{
  SO_NODE_CONSTRUCTOR(T3VEBody);

  show_drag_ = show_drag;
  // todo: impl dragger!?
  drag_ = NULL;
}

T3VEBody::~T3VEBody()
{
  
}

/////////////////////////////////////////////
//	Space

SO_NODE_SOURCE(T3VESpace);

void T3VESpace::initClass()
{
  SO_NODE_INIT_CLASS(T3VESpace, T3NodeParent, "T3NodeParent");
}

T3VESpace::T3VESpace(void* world)
:inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VESpace);
}

T3VESpace::~T3VESpace()
{
  
}

/////////////////////////////////////////////
//	Static

SO_NODE_SOURCE(T3VEStatic);

void T3VEStatic::initClass()
{
  SO_NODE_INIT_CLASS(T3VEStatic, T3NodeLeaf, "T3NodeLeaf");
}

T3VEStatic::T3VEStatic(void* bod, bool show_drag)
:inherited(bod)
{
  SO_NODE_CONSTRUCTOR(T3VEStatic);

  show_drag_ = show_drag;
  // todo: impl dragger!?
  drag_ = NULL;
}

T3VEStatic::~T3VEStatic()
{
  
}

