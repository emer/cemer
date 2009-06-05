// Copyright, 1995-2007, Regents of the University of Colorado,
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
#include <Inventor/nodes/SoTransform.h>

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

extern void T3VEBody_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

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
  drag_ = NULL;
  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);

    txfm_shape()->translation.connectFrom(&drag_->dragger_->translation);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3VEBody_DragFinishCB, (void*)this);
    insertChildBefore(topSeparator(), drag_, txfm_shape());
  }
}

T3VEBody::~T3VEBody()
{
  
}

/////////////////////////////////////////////
//	ObjCarousel

SO_NODE_SOURCE(T3VEObjCarousel);

void T3VEObjCarousel::initClass()
{
  SO_NODE_INIT_CLASS(T3VEObjCarousel, T3VEBody, "T3VEBody");
}

T3VEObjCarousel::T3VEObjCarousel(void* bod, bool show_drag)
 :inherited(bod, show_drag)
{
  SO_NODE_CONSTRUCTOR(T3VEObjCarousel);

  obj_switch_ = new SoSwitch;
  obj_switch_->whichChild = -1; // no cameras!
}

T3VEObjCarousel::~T3VEObjCarousel()
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

extern void T3VEStatic_DragFinishCB(void* userData, SoDragger* dragger);
// defined in qtso

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
  drag_ = NULL;
  if(show_drag_) {
    drag_ = new T3TransformBoxDragger(0.06f, .04f, .03f);

    txfm_shape()->translation.connectFrom(&drag_->dragger_->translation);
    txfm_shape()->rotation.connectFrom(&drag_->dragger_->rotation);
    txfm_shape()->scaleFactor.connectFrom(&drag_->dragger_->scaleFactor);

    drag_->dragger_->addFinishCallback(T3VEStatic_DragFinishCB, (void*)this);
    insertChildBefore(topSeparator(), drag_, txfm_shape());
  }
}

T3VEStatic::~T3VEStatic()
{
  
}

