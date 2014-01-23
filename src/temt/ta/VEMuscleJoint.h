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

#ifndef VEMuscleJoint_h
#define VEMuscleJoint_h 1

// parent includes:
#include <VEJoint>

// member includes:
#include <VELambdaMuscle>

// declare all other types mentioned but not required to include:


taTypeDef_Of(VEMuscleJoint);

class TA_API VEMuscleJoint : public VEJoint {
  // a Lambda-model (Gribble et al, 1998) arm joint -- updates forces at every time step, in GetVAlsFromODE, applies them using SetForce
INHERITED(VEJoint)
public:
  VELambdaMuscle        flexor; // #SHOW_TREE flexor muscle
  VELambdaMuscle        extensor; // #SHOW_TREE extensor muscle
  VELambdaMuscle        flexor2; // #SHOW_TREE flexor muscle for joint 2
  VELambdaMuscle        extensor2; // #SHOW_TREE extensor muscle for joint 2

  float                 targ_norm_angle; // #READ_ONLY #SHOW current target normalized angle
  float                 targ_angle; // #READ_ONLY #SHOW current target raw angle
  float                 targ_norm_angle2; // #READ_ONLY #SHOW #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 current target normalized angle 2
  float                 targ_angle2; // #READ_ONLY #SHOW #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 current target raw angle 2

  float                 co_contract_pct; // #READ_ONLY #SHOW current co-contraction pct

  virtual void  SetTargAngle(float trg_angle, float co_contract, float trg_angle2 = 0.0f);
  // #BUTTON #CAT_Force set target angle for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put (at least around .2 is needed, with .5 being better, to prevent big oscillations)

  virtual void  SetTargNormAngle(float trg_norm_angle, float co_contract,
                                 float trg_norm_angle2 = 0.0f);
  // #BUTTON #CAT_Force set normalized target angle (0 = lo stop, 1 = hi stop) for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put (at least around .2 is needed, with .5 being better, to prevent big oscillations)

  void Init() CPP11_OVERRIDE;
  void CurFromODE(bool updt_disp = false) CPP11_OVERRIDE;

  TA_SIMPLE_BASEFUNS(VEMuscleJoint);
protected:
  void  UpdateAfterEdit_impl();

private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // VEMuscleJoint_h
