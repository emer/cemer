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

#ifndef VEJointMotor_h
#define VEJointMotor_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API VEJointMotor : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env joint motor parameters, including servo system -- drives joint into specified position -- forces computed and applied during the CurFromODE call, using the motor system (be sure to set f_max!)
INHERITED(taOBase)
public:
  bool          motor_on;       // turn on motor mechanism, defined by subsequent parameters
  float         vel;            // #CONDSHOW_ON_motor_on target joint velocity to achieve (angular or linear) -- set to 0 to provide a resistive damping force
  float         f_max;          // #CONDSHOW_ON_motor_on maximum force or torque to drive the joint to achieve desired velocity
  bool          servo_on;       // #CONDSHOW_ON_motor_on turn on servo mechanism, defined by subsequent parameters
  float         trg_pos;        // #CONDSHOW_ON_servo_on servo: target joint position to drive toward -- IMPORTANT: do not get too close to the stops with this, as it can cause numerical problems -- leave a .02 or so buffer
  float         gain;           // #CONDSHOW_ON_servo_on servo: how high to set the velocity on each step to move toward the target position: vel = gain * (trg_pos - pos) -- this can be quite high in fact: 20 or 50 have worked in various models, depending on the stepsize, masses involved, etc

  TA_SIMPLE_BASEFUNS(VEJointMotor);
protected:
  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // VEJointMotor_h
