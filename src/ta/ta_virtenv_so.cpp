/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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

