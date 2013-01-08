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

#ifndef taAxisAngle_h
#define taAxisAngle_h 1

// parent includes:
#include <taVector3f>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taAxisAngle: public taVector3f {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP 3-d rotation data, xyz specify the rotation axis
  INHERITED(taVector3f)
public:
  float         rot; // rotation angle, in radians

  inline void   SetXYZR(float xx, float yy, float zz, float rr) {
    x = xx; y = yy; z = zz; rot = rr;
  }
  inline void   GetXYZR(float& xx, float& yy, float& zz, float& rr) {
    xx = x; yy = y; zz = z; rr = rot;
  }

  void	FromEuler(float theta_x, float theta_y, float theta_z);
  // #CAT_Rotation #BUTTON convert from Euler angles

  void	RotateAxis(float x_axis, float y_axis, float z_axis, float rot_ang);
  // #CAT_Rotation #BUTTON rotate this rotation by given axis rotation parameters
  void	RotateEuler(float theta_x, float theta_y, float theta_z);
  // #CAT_Rotation #BUTTON rotate this rotation by given rotation parameters
  void	RotateXYZ(float& x, float& y, float& z);
  // #CAT_Rotation rotate x,y,z vector according to the current rotation parameters
  void	RotateVec(taVector3f& vec);
  // #CAT_Rotation rotate vector according to the current rotation parameters

  String        GetStr() const {return taVector3f::GetStr() + ", " + String(rot); }

  taAxisAngle(float xx, float yy, float zz, float rr) {SetXYZR(xx, yy, zz, rr); }
  taAxisAngle(int xx, int yy, int zz, float rr)       {SetXYZR(xx, yy, zz, rr); }

  taAxisAngle& operator=(const taQuaternion& cp);

  TA_BASEFUNS_LITE(taAxisAngle);
private:
  void                  Copy_(const taAxisAngle& cp)  {rot = cp.rot;}
  void                  Initialize() { z = 1.0f; rot = 0.0f;}
  void                  Destroy() {}
};

#endif // taAxisAngle_h
