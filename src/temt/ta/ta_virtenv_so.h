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

#ifndef TA_VIRTENV_SO_H
#define TA_VIRTENV_SO_H

#include "ta_TA_type.h"

/////////////////////////////////////////////////////////////////////////
//		So Inventor Code

#include "t3node_so.h"

class SoOffscreenRenderer; // #IGNORE
class SoSwitch;		   // #IGNORE
class SoDirectionalLight;  // #IGNORE

class TA_API T3VEWorld : public T3NodeParent {
  // world parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEWorld);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEWorld(void* world = NULL);

  void			setSunLightDir(float x_dir, float y_dir, float z_dir);
  void			setSunLightOn(bool on);
  SoDirectionalLight* 	getSunLight()	  { return sun_light; }

  void			setCamLightDir(float x_dir, float y_dir, float z_dir);
  void			setCamLightOn(bool on);
  SoDirectionalLight* 	getCamLight()	  { return cam_light; }

  SoGroup*		getLightGroup() { return light_group; }
  SoSwitch*		getCameraSwitch() { return camera_switch; }
  SoSwitch*		getTextureSwitch() { return textures; }

protected:
  SoDirectionalLight* 	sun_light;
  SoDirectionalLight* 	cam_light;
  SoGroup*		light_group;
  SoSwitch*		camera_switch; // switching between diff cameras
  SoSwitch*		textures;      // group of shared textures (always switched off -- used by nodes)
  
  ~T3VEWorld();
};

class TA_API T3VEObject : public T3NodeParent {
  // object parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEObject);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEObject(void* obj = NULL);

protected:
  ~T3VEObject();
};

class TA_API T3VEBody : public T3NodeLeaf {
  // body for virtual environment 
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3VEBody);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEBody(void* bod = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3VEBody();
};

class TA_API T3VESpace : public T3NodeParent {
  // space parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VESpace);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VESpace(void* obj = NULL);

protected:
  ~T3VESpace();
};

class TA_API T3VEStatic : public T3NodeLeaf {
  // static item for virtual environment 
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3VEStatic);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEStatic(void* stat = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3VEStatic();
};

#endif // TA_VIRTENV_SO_H
