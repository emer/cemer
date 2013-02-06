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

#include "VELinearMuscle.h"
#include <VEArm>


void VELinearMuscle::Initialize() {
  gain = 1;
  IPprox.SetXYZ(0.0f,0.0f,0.0f);
  IPdist.SetXYZ(0.1f,0.1f,0.1f);
  p3.SetXYZ(0.0f,0.0f,0.0f);
  bend = false;
  old_length2 = old_length1 = Length();
}

void VELinearMuscle::Init(taVector3f prox, taVector3f dist, float MrG) {
  gain = MrG;
  IPprox = prox;
  IPdist = dist;
  p3.SetXYZ(0.0f,0.0f,0.0f);
  bend = false;
  old_length2 = old_length1 = Length();
}

void VELinearMuscle::Init(taVector3f prox, taVector3f dist, float MrG, taVector3f pp3, bool bending) {
  gain = MrG;
  IPprox = prox;
  IPdist = dist;
  p3 = pp3;
  bend = bending;
  old_length2 = old_length1 = Length();
}

void VELinearMuscle::Destroy() { }

taVector3f VELinearMuscle::Contract(float stim) {
  taVector3f force_vec;
  if(bend)
    force_vec = p3 - IPdist;
  else
    force_vec = IPprox - IPdist;  // vector points from distal to proximal

  force_vec.MagNorm();  // force_vec has now magnitude = 1
  return force_vec*gain*MAX(stim,0);
}

float VELinearMuscle::Length() {
  if(bend) // if muscle wraps around bending line
    return (IPprox - p3).Mag() + (p3 - IPdist).Mag();
  else
    return (IPprox - IPdist).Mag();
}

float VELinearMuscle::Speed() {
  float length = Length();
  VEArm* army = GetArm();
  float step = army->world_step; // copy of VEWorld stepsize
  return (length - old_length2)/(2*step);  // 3 point rule
}

void VELinearMuscle::UpOld() { 
    old_length2 = old_length1;
    old_length1 = Length();
}

void VELinearMuscle::InitBuffs() {
   old_length2 = old_length1 = Length();
}

float VELinearMuscle::Old_Length() { 
// this function has no use in VELinearMuscle
  return Length();
}

float VELinearMuscle::Old_Speed() { 
// this function has no use in VELinearMuscle
  return Speed();
} 

