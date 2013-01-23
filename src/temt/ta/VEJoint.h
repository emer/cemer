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

#ifndef VEJoint_h
#define VEJoint_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <VEBody>
#include <taVector3f>
#include <ODEIntParams>


// declare all other types mentioned but not required to include:
class VEWorld; // 
TypeDef_Of(VEWorld);

TypeDef_Of(VEJointStops);

class TA_API VEJointStops : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env joint stop parameters
INHERITED(taOBase)
public:
  bool          stops_on;       // turn on stops -- otherwise not used
  float         lo;             // #CONDSHOW_ON_stops_on stop for low angle or position value of joint
  float         hi;             // #CONDSHOW_ON_stops_on stop for high angle or position value of joint
  float         def;            // #CONDSHOW_ON_stops_on default angle or position value of joint -- where it likes to be
  float         bounce;         // #CONDSHOW_ON_stops_on how bouncy is the joint (0 = no bounce, 1 = maximum bounce)
  float         def_force;      // #CONDSHOW_ON_stops_on how much force to apply to return joint to default position -- effectively adds springs to the joint that pull it back to the default position -- NOTE: must call ApplyForce to have this computed and updated

  inline float  Range() const           { return (hi - lo); }
  inline float  Scale() const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }

  float Normalize(float val) const      { return (val - lo) * Scale(); }
  // normalize given value to 0-1 range given current in hi

  float Project(float val) const        { return lo + (val * Range()); }
  // project a normalized value into the current lo-hi range

  TA_SIMPLE_BASEFUNS(VEJointStops);
// protected:
//   void       UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { };
};

TypeDef_Of(VEJointMotor);

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

TypeDef_Of(ODEJointParams);

class TA_API ODEJointParams : public ODEIntParams {
  // ODE integration parameters for joints
INHERITED(ODEIntParams)
public:
  float         no_stop_cfm;    // #DEF_1e-05 (0-1, 1e-9 - 1 useful range) constraint force mixing parameter when not at a stop: how "soft" is the constraint (0 = hard, 1 = soft)
  float         fudge;          // #DEF_1 (0-1) fudge factor for reducing force when a motor is present -- reduce this value if there is excessive jumping at the joint

  void  Initialize();
  void  Destroy()       { };
  SIMPLE_COPY(ODEJointParams);
  TA_BASEFUNS(ODEJointParams);
// protected:
//   void       UpdateAfterEdit_impl();
};

TypeDef_Of(VEJoint);

class TA_API VEJoint : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vejnt a virtual environment joint, which connects two bodies
INHERITED(taNBase)
public:
  enum JointFlags { // #BITS flags for joints
    JF_NONE             = 0, // #NO_BIT
    OFF                 = 0x0001, // joint is not functional and turned off
    FEEDBACK            = 0x0002, // collect feedback information about the joint
    USE_ODE_PARAMS      = 0x0004, // use special ODE parameters for this joint (else uses world settings)
  };

  enum JointType {      // type of joint: Important -- must be sync'd with joint types in ode/common.h!!!
    NO_JOINT = 0,       // no joint type set
    BALL,               // ball joint -- no constraints on relative orientation
    HINGE,              // hinged -- only bends in one axis
    SLIDER,             // slider -- moves
    UNIVERSAL = 5,      // hinged in 2 axes -- also like a ball joint but transmits torque
    HINGE2,             // two hinges -- first one is like a suspension with a soft constraint, and second one can be used for a rotating wheel or similar
    FIXED,              // fixed -- use of this is discouraged (just redefine body shape), but it can be quite useful
  };

  // note this seems to be missing in 0.7:
  //    PR = dJointTypePR,              // rotoide & prismatic

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*         joint_id;       // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the joint (cast to a dJointID which is dxjoint*)
  JointFlags    flags;          // joint flags
  VEBodyRef     body1;          // #SCOPE_VEWorld first body in the joint
  VEBodyRef     body2;          // #SCOPE_VEWorld second body in the joint
  JointType     joint_type;     // type of joint
  taVector3f    anchor;         // anchor location for joint, specified RELATIVE TO BODY1 (note this is different from ODE -- we just add body1's position to this anchor position)
  taVector3f    axis;           // #CONDSHOW_OFF_joint_type:BALL axis orientation vector
  taVector3f    axis2;          // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 second axis for universal joint and hinge2 -- for universal, the first axis should be 1,0,0 and second 0,1,0 if those are the two axes being used -- otherwise a dRFrom2Axes zero length vector error will occur!
  float         vis_size;       // visual size of the joint, for when show_joint is active in viewer -- has NO implication for function whatsoever -- this is typically the length of the axis rod
  VEJointStops  stops;          // stop parameters for first joint: where the joint will stop (specific meaning is joint-dependent)
  VEJointStops  stops2;         // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 stop parameters for second joint: where the joint will stop (specific meaning is joint-dependent)
  VEJointMotor  motor;          // motor parameters that drive a powered joint
  VEJointMotor  motor2;         // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 motor parameters for second joint that drive a powered joint
  ODEJointParams ode_params;    // #CONDSHOW_ON_flags:USE_ODE_PARAMS ode integration parameters specifically for joints -- only used when USE_ODE_PARAMS is set
  ODEIntParams  suspension;     // #CONDSHOW_ON_joint_type:HINGE2 ode integration parameters for the hinge2 joint

  ////////////////////////////////////////////////////
  // feedback information -- only if feedback flag is set
  float         pos;            // #READ_ONLY #SHOW probed position value (joint dependent; could be angle)
  float         pos_norm;       // #READ_ONLY #SHOW normalized position value (joint dependent; could be angle) -- if stops.stops_on, then 0 = lo stop, 1 = hi stop
  float         vel;            // #READ_ONLY #SHOW probed velocity value (joint dependent; could be angle)
  float         pos2;           // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW probed position value for 2nd axis (joint dependent; could be angle)
  float         pos2_norm;      // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW normalized position value for 2nd axis (joint dependent; could be angle) -- if stops.stops_on, then 0 = lo stop, 1 = hi stop
  float         vel2;           // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW probed velocity value for 2nd axis (joint dependent; could be angle)

  taVector3f    cur_force1;     // #READ_ONLY #SHOW force that joint applies to body 1
  taVector3f    cur_torque1;    // #READ_ONLY #SHOW torque that joint applies to body 1
  taVector3f    cur_force2;     // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW force that joint applies to body 2
  taVector3f    cur_torque2;    // #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW torque that joint applies to body 2

  override String       GetDesc() const { return desc; }
  override int          GetEnabled() const {  return !HasJointFlag(OFF); }
  override void         SetEnabled(bool value) { SetJointFlagState(OFF, !value); }

  inline void           SetJointFlag(JointFlags flg)   { flags = (JointFlags)(flags | flg); }
  // set joint flag state on
  inline void           ClearJointFlag(JointFlags flg) { flags = (JointFlags)(flags & ~flg); }
  // clear joint flag state (set off)
  inline bool           HasJointFlag(JointFlags flg) const { return (flags & flg); }
  // check if joint flag is set
  inline void           SetJointFlagState(JointFlags flg, bool on)
  { if(on) SetJointFlag(flg); else ClearJointFlag(flg); }
  // set joint flag state according to on bool (if true, set flag, if false, clear it)

  inline bool           HasTwoAxes()
  { return (joint_type == UNIVERSAL || joint_type == HINGE2); }
  // determine if joint has two axes of movement (else 1)

  virtual VEWorld* GetWorld();  // #CAT_ODE get world object (parent of this guy)
  virtual void* GetWorldID();   // #CAT_ODE get world id value

  virtual bool  CreateODE();    // #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void  DestroyODE();   // #CAT_ODE destroy object in ode (if created)

  virtual void  Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void  SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void  CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing

  virtual void  ApplyForce(float force1, float force2 = 0.0f);
  // #BUTTON #CAT_Force apply force(s) (or torque(s) as the case may be) to the joint (only good for next time step)
  virtual void  ApplyMotor(float vel1, float f_max1, float vel2 = 0.0f, float f_max2 = 0.0f);
  // #BUTTON #CAT_Force apply motor target velocity and max force parameters to joint (persist until further changes) -- set f_max = 0 to turn off -- automatically turns ON motor_on and OFF servo_on (servo otherwise takes control of motor parameters)
  virtual void  ApplyServo(float trg_pos1, float trg_pos2 = 0.0f);
  // #BUTTON #CAT_Force set servo_on and update target positions for the servos -- servo control automatically applied when the system is stepped
  virtual void  ApplyServoNorm(float trg_norm_pos1, float trg_norm_pos2 = 0.0f, float stop_buffer=0.02f);
  // #BUTTON #CAT_Force set servo_on and update target positions for the servos using *normalized* values relative to lo-hi joint stops, with stop_buffer bounds to prevent numerical errors, so it doesn't go lower than lo + stop_buffer and higher than hi - stop_buffer -- servo control automatically applied when the system is stepped

  virtual void  Init_Anchor(); // #CAT_ODE set ODE anchor(s) and axes to the anchor and axisX values
  virtual void  Init_Stops(); // #CAT_ODE set stop(s) (including suspension for hinge 2)
  virtual void  Init_Motor(); // #CAT_ODE set motor params
  virtual void  Init_ODEParams(); // #CAT_ODE set ode integration parameters (erp, cfm)

  bool  IsCurType()  { return (joint_type == cur_type); }
  // #CAT_ODE is the ODE guy actually configured for the current joint type or not?

  SIMPLE_COPY_UPDT_PTR_PAR(VEJoint, VEWorld);
  SIMPLE_INITLINKS(VEJoint);
  override void CutLinks();
  TA_BASEFUNS(VEJoint);
protected:
  JointType             cur_type;       // current type that was previously set

#ifndef __MAKETA__
  dJointFeedback        ode_fdbk_obj;   // #IGNORE ode feedback object
#endif
protected:
  void  UpdateAfterEdit_impl();

  //  override CheckConfig_impl() // todo
private:
  void  Initialize();
  void  Destroy();
};

SmartRef_Of(VEJoint); // VEJointRef

#endif // VEJoint_h
