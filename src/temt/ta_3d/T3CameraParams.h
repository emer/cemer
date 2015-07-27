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

#ifndef T3CameraParams_h
#define T3CameraParams_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(T3CameraParams);

class TA_API T3CameraParams : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_T3D parameters for a 3D camera
INHERITED(taOBase)
public:
  float         field_of_view;  // field of view in degrees of the camera -- how wide or narrow the field is
  float         near;           // #DEF_0.1 near distance of camera -- closest things can be seen
  float         focal;          // focal distance of camera -- where is it focused on in scene?
  float         far;            // far distance of camera -- furthest things that can be seen

  TA_SIMPLE_BASEFUNS(T3CameraParams);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // T3CameraParams_h
