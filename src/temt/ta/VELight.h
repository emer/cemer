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

#ifndef VELight_h
#define VELight_h 1

// parent includes:
#include <VEBody>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taVector3f>

// declare all other types mentioned but not required to include:
class SoLight; // 

class TA_API VELightParams : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env light parameters
INHERITED(taOBase)
public:
  bool          on;             // is the light turned on?
  float         intensity;      // #CONDSHOW_ON_on (0-1) how bright is the light
  taColor       color;          // #CONDSHOW_ON_on color of light

  TA_SIMPLE_BASEFUNS(VELightParams);
private:
  void  Initialize();
  void  Destroy()       { };
};


class TA_API VELight : public VEBody {
  // virtual environment light -- a body that contains a light source -- body shape is not rendered, but mass/inertia etc is used if part of a non-fixed object -- light only affects items after it in the list of objects!
INHERITED(VEBody)
public:
  enum LightType {
    DIRECTIONAL_LIGHT,          // shines in a given direction, rotation is used for the direction, but position is not -- fastest rendering speed
    POINT_LIGHT,                // radiates in all directions, uses position for location
    SPOT_LIGHT,                 // shines in a given direction from a given position
  };

#ifdef __MAKETA__
  LongAxis      long_axis;      // #READ_ONLY #HIDDEN direction of the long axis of the body (where length is oriented) -- must always be LONG_Z for a light
#endif

  LightType     light_type;     // type of light
  VELightParams light;          // light parameters
  float         drop_off_rate;  // #CONDSHOW_ON_light_type:SPOT_LIGHT (0-1) how fast light drops off with increasing angle from the direction angle
  float         cut_off_angle;  // #CONDSHOW_ON_light_type:SPOT_LIGHT (45 default) angle in degrees from the direction vector where there will be no light
  taVector3f    dir_norm;       // #READ_ONLY #SHOW normal vector for where the camera is facing

  virtual SoLight*      CreateLight();
  // create the So light of correct type
  virtual void          ConfigLight(SoLight* lgt);
  // config So light parameters
  virtual bool          UpdateLight();
  // #BUTTON if environment is already initialized and viewed, this will update the light in the display based on current settings

  override void Init();
  override void CurFromODE(bool updt_disp = false);

  TA_SIMPLE_BASEFUNS(VELight);
private:
  void  Initialize();
  void  Destroy() { };
};

SmartRef_Of(VELight,TA_VELight); // VELightRef

#endif // VELight_h
