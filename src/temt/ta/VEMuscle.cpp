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

#include "VEMuscle.h"
#include <VEArm>

///////////////////////////////////////////////////////////////
//   VEMuscle: skeleton class for muscles

void VEMuscle::Initialize() {
  IPprox.SetXYZ(0.0f,0.0f,0.0f);
  IPdist.SetXYZ(0.1f,0.1f,0.1f);
  p3.SetXYZ(0.0f,0.0f,0.0f);
  bend = false;
}

void VEMuscle::Destroy() { }

VEArm* VEMuscle::GetArm() {
  return GET_MY_OWNER(VEArm); // somehow Randy's macro does the trick
}

taVector3f VEMuscle::Contract(float stim) {
  taVector3f force_vec;
  if(bend)
    force_vec = p3 - IPdist;
  else
    force_vec = IPprox - IPdist;  // vector points from distal to proximal

  force_vec.MagNorm();  // force_vec has now magnitude = 1
  return force_vec*MAX(stim,0);
}

float VEMuscle::Length() {
  if(bend) // if muscle wraps around bending line
    return (IPprox - p3).Mag() + (p3 - IPdist).Mag();
  else
    return (IPprox - IPdist).Mag();
}

float VEMuscle::Speed() {
  return 0.0f;
}

void VEMuscle::UpOld() { } // this class has no old values to update

void VEMuscle::InitBuffs() {  }

float VEMuscle::Old_Length() { 
// this function has no use in VEMuscle
  return Length();
}

float VEMuscle::Old_Speed() { 
// this function has no use in VEMuscle
  return Speed();
}

