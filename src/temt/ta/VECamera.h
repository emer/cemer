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

#ifndef VECamera_h
#define VECamera_h 1

// parent includes:
#include <VEBody>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taVector3f>
#include <taVector2i>
#include <VELightParams>

// declare all other types mentioned but not required to include:
class SoPerspectiveCamera; // #IGNORE

#ifdef TA_OS_WIN
// following for msvc
# ifdef near
#   undef near
# endif
# ifdef far
#   undef far
# endif
// # ifdef GetObject
// #   undef GetObject
// # endif
#endif


TypeDef_Of(VECameraDists);

class TA_API VECameraDists : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env camera distances
INHERITED(taOBase)
public:
  float         near;           // #DEF_0.1 near distance of camera -- closest things can be seen
  float         focal;          // focal distance of camera -- where is it focused on in scene?
  float         far;            // far distance of camera -- furthest things that can be seen

  TA_SIMPLE_BASEFUNS(VECameraDists);
private:
  void  Initialize();
  void  Destroy()       { };
};

TypeDef_Of(VECamera);

class TA_API VECamera : public VEBody {
  // virtual environment camera -- a body that contains a camera -- position and orientation are used to point the camera -- body shape always a cylinder, with LONG_Z axis, but mass/inertia etc is used if part of a non-fixed object -- camera must be selected in the VEWorld for it to actually be used to render images!
INHERITED(VEBody)
public:

#ifdef __MAKETA__
  Shape         shape;          // #READ_ONLY #HIDDEN shape for camera must always be a cylinder
  LongAxis      long_axis;      // #READ_ONLY #HIDDEN direction of the long axis of the body (where length is oriented) -- must always be LONG_Z for a camera
  taVector3f    box;            // #READ_ONLY #HIDDEN not relevant
#endif

  taVector2i    img_size;       // size of image to record from camera -- IMPORTANT -- if you will be using multiple cameras, it is very important that they all are the same size, otherwise the rendering will consume a lot of memory because a new buffer has to be created and destroyed each time (a shared buffer is used for all cameras)
  bool          color_cam;      // if true, get full color images (else greyscale)
  VECameraDists view_dist;      // distances that are in view of the camera
  float         field_of_view;  // field of view of camera (angle in degrees) -- how much of scene is it taking in
  VELightParams light;          // directional "headlight" associated with the camera -- ensures that whatever is being viewed can be seen (but makes lighting artificially consistent and good)
  taVector3f    dir_norm;       // #READ_ONLY #SHOW normal vector for where the camera is facing

  virtual void          ConfigCamera(SoPerspectiveCamera* cam);
  // #IGNORE config So camera parameters

  override void Init();
  override void CurFromODE(bool updt_disp = false);

  TA_SIMPLE_BASEFUNS(VECamera);
private:
  void  Initialize();
  void  Destroy() { };
};

SmartRef_Of(VECamera); // VECameraRef

#endif // VECamera_h
