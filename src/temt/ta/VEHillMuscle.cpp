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

#include "VEHillMuscle.h"
#include <VEArm>

TA_BASEFUNS_CTORS_DEFN(VEHillMuscle);

void VEHillMuscle::Initialize() {
  IPprox.SetXYZ(0.0f,0.0f,0.0f);
  IPdist.SetXYZ(0.1f,0.1f,0.1f);
  p3.SetXYZ(0.0f,0.0f,0.0f);
  bend = false;
  mu = 0.06f;
  c = 112.0f;
  rho = 15.0f; 
  tau = 0.015f;
  d = 0.025f;
  f1 = 0.82f;
  f2 = 0.5f;
  f3 = 0.43f;
  f4 = 58.0f;
  k = 200.0f;
  r = 0.12f;
  d_steps = 5;
  M = 0;
  dM = 0;
  tau2 = tau*tau;
  len_buf.Reset();
  dlen_buf.Reset();
}

void VEHillMuscle::Destroy() { }

taVector3f VEHillMuscle::Contract(float stim) {

//=== OBTAINING THE MAGNITUDE OF THE FORCE ===
  VEArm* army = GetArm();
  float step = army->world_step; // copy of VEWorld stepsize

  stim = MIN(stim,8.0f); // clipping stim 

  float m = rho*expf(c*MAX(stim,0.0f) - 1.0f);  // equation 4 of Gribble et al.
  //float m = rho*MAX(stim,0.0f);
  M += step*dM;       // these two lines implement equation 5 of Gribble et al.
  dM += step*(m - M -2.0f*tau*dM)/tau2;
  //M += step*(m-M);
  //M = m;
  float force = M*(f1 + f2*atanf(f3 + f4*Speed())); // + k*(Length()-r); // eq. 6
  //float force = M*f1;
  force = MIN(force,1000.0f); // clipping force

//=== OBTAINING THE VECTOR OF THE FORCE ===
  taVector3f force_vec;
  if(bend)
    force_vec = p3 - IPdist;
  else
    force_vec = IPprox - IPdist;  // vector points from distal to proximal

  force_vec.MagNorm();  // force_vec has now magnitude = 1

  return force_vec*force;
}

float VEHillMuscle::Length() {
   if(bend) // if muscle wraps around bending line
    return (IPprox - p3).Mag() + (p3 - IPdist).Mag();
  else
    return (IPprox - IPdist).Mag(); 
}

float VEHillMuscle::Speed() {
  float length = Length();
  VEArm* army = GetArm();
  float step = army->world_step; // copy of VEWorld stepsize
  return (length - len_buf.CircSafeEl(d_steps-2))/(2.0f*step);  // 3 point rule
}

void VEHillMuscle::UpOld() {

// these lines add a new element to the circular buffers
  len_buf.CircAddLimit(Length(), d_steps);
  dlen_buf.CircAddLimit(Speed(), d_steps);

}

void VEHillMuscle::InitBuffs() {
  
  VEArm* army = GetArm();
  float step = army->world_step; // copy of VEWorld stepsize

  d_steps = ceil(d/step); // duration of d in time steps
  len_buf.SetSize(d_steps);
  dlen_buf.SetSize(d_steps);

  float len = Length();
  for(int j=0; j<d_steps; j++) {  
    len_buf.CircAddLimit(len, d_steps);
    dlen_buf.CircAddLimit(0.0f, d_steps);
  }
  
}

float VEHillMuscle::Old_Length() {
  return len_buf.CircSafeEl(0);
}

float VEHillMuscle::Old_Speed() {
  return dlen_buf.CircSafeEl(0);
}

