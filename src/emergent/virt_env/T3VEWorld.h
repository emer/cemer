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
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:

#ifdef TA_QT3D

class E_API T3VEWorld : public T3NodeParent {
  // world parent for virtual environment
  Q_OBJECT
  INHERITED(T3NodeParent)
public:
  bool                  shadows;             // cast shadows?

  void                  setSunLightDir(float x_dir, float y_dir, float z_dir);
  void                  setSunLightOn(bool on);
  // SoDirectionalLight*   getSunLight()     { return sun_light; } // #IGNORE 

  void                  setCamLightDir(float x_dir, float y_dir, float z_dir);
  void                  setCamLightOn(bool on);
  // SoDirectionalLight*   getCamLight()     { return cam_light; } // #IGNORE 

  // SoGroup*              getLightGroup() { return light_group; }  // #IGNORE 
  // SoSwitch*             getCameraSwitch() { return camera_switch; } // #IGNORE 
  // SoSwitch*             getTextureSwitch() { return textures; } // #IGNORE 
  // SoSwitch*             getTextureXformSwitch() { return texture_xforms; } // #IGNORE 

  // SoShadowGroup*        getShadowGroup() { return shadow_group; } // #IGNORE
  
  void                  setShadows(bool on, float precision, float quality,
                                   float smoothing, float near_radius,
                                   float far_radius, float threshold, float epsilon);
  bool                  getShadows() { return shadows; }

  // SoSeparator*	childNodes() override;

  T3VEWorld(Qt3DNode* par = NULL, T3DataView* world = NULL);
  ~T3VEWorld();

// protected:
//   SoShadowGroup*        shadow_group; // this owns everything..
//   SoDirectionalLight*   sun_light;
//   SoDirectionalLight*   cam_light;
//   SoGroup*              light_group;
//   SoSwitch*             camera_switch; // switching between diff cameras
//   SoSwitch*             textures;      // group of shared textures (always switched off -- used by nodes)
//   SoSwitch*             texture_xforms;      // group of shared textures (always switched off -- used by nodes)
//   SoShadowStyle*        shadow_style;

};

#else // TA_QT3D

class SoDirectionalLight; // #IGNORE
class SoGroup; // 
class SoSwitch; //
class SoShadowStyle; //
class SoShadowGroup; //

eTypeDef_Of(T3VEWorld);

class E_API T3VEWorld : public T3NodeParent {
  // world parent for virtual environment
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  TA_SO_NODE_HEADER(T3VEWorld);
#endif // def __MAKETA__
public:
  static void   initClass();

  T3VEWorld(T3DataView* world = NULL);

  void                  setSunLightDir(float x_dir, float y_dir, float z_dir);
  void                  setSunLightOn(bool on);
  SoDirectionalLight*   getSunLight()     { return sun_light; } // #IGNORE 

  void                  setCamLightDir(float x_dir, float y_dir, float z_dir);
  void                  setCamLightOn(bool on);
  SoDirectionalLight*   getCamLight()     { return cam_light; } // #IGNORE 

  SoGroup*              getLightGroup() { return light_group; }  // #IGNORE 
  SoSwitch*             getCameraSwitch() { return camera_switch; } // #IGNORE 
  SoSwitch*             getTextureSwitch() { return textures; } // #IGNORE 
  SoSwitch*             getTextureXformSwitch() { return texture_xforms; } // #IGNORE 

  SoShadowGroup*        getShadowGroup() { return shadow_group; } // #IGNORE
  
  void                  setShadows(bool on, float precision, float quality,
                                   float smoothing, float near_radius,
                                   float far_radius, float threshold, float epsilon);
  bool                  getShadows() { return shadows; }

  SoSeparator*	childNodes() override;

protected:
  SoShadowGroup*        shadow_group; // this owns everything..
  SoDirectionalLight*   sun_light;
  SoDirectionalLight*   cam_light;
  SoGroup*              light_group;
  SoSwitch*             camera_switch; // switching between diff cameras
  SoSwitch*             textures;      // group of shared textures (always switched off -- used by nodes)
  SoSwitch*             texture_xforms;      // group of shared textures (always switched off -- used by nodes)
  SoShadowStyle*        shadow_style;
  bool                  shadows;             // cast shadows?

  ~T3VEWorld();
};

#endif // TA_QT3D

#endif // T3VEWorld_h
