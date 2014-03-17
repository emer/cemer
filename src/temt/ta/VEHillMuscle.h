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

#ifndef VEHillMuscle_h
#define VEHillMuscle_h 1

// parent includes:
#include <VEMuscle>

// member includes:
#include <float_CircBuffer>

// declare all other types mentioned but not required to include:
class VEArm; //

taTypeDef_Of(VEHillMuscle);

class TA_API VEHillMuscle : public VEMuscle {
// muscle model from Gribble et al. 1998
  INHERITED(VEMuscle)
public:
  float_CircBuffer len_buf;  // buffer with past lengths. Actualized by UpdateIPs 
  float_CircBuffer dlen_buf;  // buffer with past speeds. Actualized by UpdateIPs 
  //static int d_steps;      // length of the delay d below in time steps. Initialized in ConfigArm
  int d_steps;   // length of the delay d below in time steps. Initialized in ConfigArm
  float M;       // low pass filter variable (M in eq. 6 of Gribble et al.)
  float dM;      // derivative of low pass filter variable (M' in eq. 6 of Gribble et al.)
  float tau2;    // auxiliary variable, equal to tau*tau

  float mu;       // #DEF_0.06 [s] dependence of muscle's threshold length on velocity
  float c;        // #DEF_112  [1/m] form parameter
  float rho;      // [N] force-generating capability (proportional to cross-sectional area)
  float tau;      // #DEF_0.015 [s] calcium kinetics time constant
  float d;        // #DEF_0.025 [s] reflex delay
  float f1;       // #DEF_0.82  [-] first fitting coefficient of equation (6)
  float f2;       // #DEF_0.5  [-] second fitting coefficient of equation (6)
  float f3;       // #DEF_0.43  [-] third fitting coefficient of equation (6)
  float f4;       // #DEF_58  [s/m] fourth fitting coefficient of equation (6)
  float k;        // [N/m] passive stiffness
  float r;        // [m] resting length

  taVector3f 	Contract(float stim);
  // #IGNORE Returns the force vector (pointing towards the proximal insertion point) resulting from a given stimulation of the muscle;

  float Length();       // Returns current length of muscle
  float Speed();  	// Returns muscle's contraction speed one world stepsize ago
  void UpOld();		// update past values stored
  void InitBuffs();     // Initialize the buffers that store past values
  float Old_Length();   // Delayed value of muscle length
  float Old_Speed();    // Delayed value of muscle speed

  TA_SIMPLE_BASEFUNS(VEHillMuscle);

private:

  void Initialize();
  void Destroy();
};

SMARTREF_OF(TA_API, VEHillMuscle); // VEHillMuscleRef

#endif // VEHillMuscle_h
