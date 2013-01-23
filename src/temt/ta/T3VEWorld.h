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

#ifndef T3VEWorld_h
#define T3VEWorld_h 1

// parent includes:
#include <T3NodeParent>

// member includes:

// declare all other types mentioned but not required to include:
class SoDirectionalLight; // #IGNORE
class SoGroup; // 
class SoSwitch; // 


TypeDef_Of(T3VEWorld);

class TA_API T3VEWorld : public T3NodeParent {
  // world parent for virtual environment
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEWorld);
#endif // def __MAKETA__
public:
  static void   initClass();

  T3VEWorld(T3DataView* world = NULL);

  void                  setSunLightDir(float x_dir, float y_dir, float z_dir);
  void                  setSunLightOn(bool on);
  SoDirectionalLight*   getSunLight()     { return sun_light; }

  void                  setCamLightDir(float x_dir, float y_dir, float z_dir);
  void                  setCamLightOn(bool on);
  SoDirectionalLight*   getCamLight()     { return cam_light; }

  SoGroup*              getLightGroup() { return light_group; }
  SoSwitch*             getCameraSwitch() { return camera_switch; }
  SoSwitch*             getTextureSwitch() { return textures; }
  SoSwitch*             getTextureXformSwitch() { return texture_xforms; }

protected:
  SoDirectionalLight*   sun_light;
  SoDirectionalLight*   cam_light;
  SoGroup*              light_group;
  SoSwitch*             camera_switch; // switching between diff cameras
  SoSwitch*             textures;      // group of shared textures (always switched off -- used by nodes)
  SoSwitch*             texture_xforms;      // group of shared textures (always switched off -- used by nodes)

  ~T3VEWorld();
};

#endif // T3VEWorld_h
